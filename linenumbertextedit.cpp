#include "linenumbertextedit.hpp"
#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QMouseEvent>
#include <QUrl>


LineNumberTextEdit::LineNumberTextEdit(QWidget *parent)
        : QTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this->document(), &QTextDocument::blockCountChanged, this, &LineNumberTextEdit::updateLineNumberAreaWidth);
    connect(this, &QTextEdit::cursorPositionChanged, this, &LineNumberTextEdit::cursorPositionChangedSlot);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &LineNumberTextEdit::onScrollBarValueChanged);

    updateLineNumberAreaWidth();
}

void LineNumberTextEdit::cursorPositionChangedSlot() {
    QRect rect = this->rect();
    int dy = this->verticalScrollBar()->value();

    updateLineNumberArea(rect, dy);
    updateLineNumberAreaWidth();
}


void LineNumberTextEdit::onScrollBarValueChanged(int value)
{
    Q_UNUSED(value);
    updateLineNumberArea(QRect(), 0);
    updateLineNumberAreaWidth();
}

void LineNumberTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    int maxDigitsWidth = lineNumberAreaWidth();

    QTextCursor cursor = this->cursorForPosition(QPoint(0, event->rect().top()));
    int blockNumber = cursor.blockNumber();
    int top = (int)cursorRect(cursor).top();
    int bottom = (int)cursorRect(cursor).bottom();

    while (top <= event->rect().bottom()) {
        if (bottom >= event->rect().top()) {
            QTextTable *table = cursor.currentTable();
            if (!table && !cursor.block().text().startsWith(QStringLiteral(" "))) {
                QString number = QString::number(blockNumber + 1)+" ";
                painter.setPen(Qt::black);
                painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
                blockNumber++;
            } else {
                QTextBlock nextBlock = cursor.block().next();
                QTextCursor nextCursor(nextBlock);
                QTextTable *nextTable = nextCursor.currentTable();
                if (table != nextTable) {
                    QString number = QString::number(blockNumber + 1)+" ";
                    painter.setPen(Qt::black);
                    painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
                }
                QTextBlock block = cursor.block();
                QTextBlockFormat blockFormat = block.blockFormat();
                if (blockFormat.leftMargin() != maxDigitsWidth) {
                    blockFormat.setLeftMargin(maxDigitsWidth);
                    cursor.setBlockFormat(blockFormat);
                }
            }
        }

        if (cursor.movePosition(QTextCursor::NextBlock)) {
            top = bottom;
            bottom = (int)cursorRect(cursor).bottom();
        } else {
            break;
        }
    }
}

int LineNumberTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, this->document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    digits += 1;
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void LineNumberTextEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    lineNumberArea->update();
}


void LineNumberTextEdit::setViewportMarginsPublic(int left, int top, int right, int bottom)
{
    setViewportMargins(left, top, right, bottom);
}

void LineNumberTextEdit::updateLineNumberAreaWidth() {
    int lineNumberAreaWidthValue = lineNumberAreaWidth();
    setViewportMargins(lineNumberAreaWidthValue, 0, 0, 0);

    QTextDocument *doc = document();
    QTextOption option = doc->defaultTextOption();
    option.setTextDirection(Qt::LayoutDirectionAuto);
    option.setFlags(QTextOption::IncludeTrailingSpaces);
    option.setWrapMode(QTextOption::WordWrap);
    option.setUseDesignMetrics(false);
    option.setTabStop(40);
    doc->setDefaultTextOption(option);

    QTextBlockFormat blockFormat;
    blockFormat.setLeftMargin(lineNumberAreaWidthValue);
    QTextCursor cursor(doc);

    QTextBlock block = doc->begin();
    while (block.isValid()) {
        QTextCursor tempCursor(block);
        if (!tempCursor.currentTable()) {
            tempCursor.setBlockFormat(blockFormat);
        }
        block = block.next();
    }
}

void LineNumberTextEdit::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
}

void LineNumberTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier)
    {
        QTextCursor cursor = cursorForPosition(event->pos());
        QTextCharFormat charFormat = cursor.charFormat();
        if (charFormat.isAnchor())
        {
            QUrl url(charFormat.anchorHref());
            emit linkClicked(url);
        }
    }
    QTextEdit::mousePressEvent(event);
}

