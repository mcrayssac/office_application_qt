#include "linenumbertextedit.hpp"
#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>

LineNumberTextEdit::LineNumberTextEdit(QWidget *parent)
        : QTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this->document(), &QTextDocument::blockCountChanged, this, &LineNumberTextEdit::updateLineNumberAreaWidth);
    connect(this, &QTextEdit::cursorPositionChanged, this, &LineNumberTextEdit::cursorPositionChangedSlot);

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
}

void LineNumberTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextCursor cursor = this->cursorForPosition(QPoint(0, event->rect().top()));
    int blockNumber = cursor.blockNumber();
    int top = (int)cursorRect(cursor).top();
    int bottom = (int)cursorRect(cursor).bottom();

    while (top <= event->rect().bottom()) {
        if (bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1) + " "; // Ajouter un espace à la fin du numéro
            painter.setPen(Qt::black);
            // Changer l'alignement de Qt::AlignRight à Qt::AlignLeft
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        if (cursor.movePosition(QTextCursor::NextBlock)) {
            blockNumber = cursor.blockNumber();
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
    blockFormat.setLeftMargin(lineNumberAreaWidthValue); // Set the left margin here
    QTextCursor cursor(doc);
    cursor.setBlockFormat(blockFormat);
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