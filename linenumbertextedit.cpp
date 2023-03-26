#include "linenumbertextedit.hpp"
#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QMouseEvent>
#include <QUrl>
#include <QInputDialog>
#include <QMenu>


LineNumberTextEdit::LineNumberTextEdit(QWidget *parent)
        : QTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this->document(), &QTextDocument::blockCountChanged, this, &LineNumberTextEdit::updateLineNumberAreaWidth);
    connect(this, &QTextEdit::cursorPositionChanged, this, &LineNumberTextEdit::cursorPositionChangedSlot);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &LineNumberTextEdit::onScrollBarValueChanged);

    updateLineNumberAreaWidth();

    previousText = this->toPlainText();
    connect(this, &QTextEdit::textChanged, this, &LineNumberTextEdit::textChangedSlot);
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

    int commentIconWidth = fontMetrics().height();

    while (top <= event->rect().bottom()) {
        if (bottom >= event->rect().top()) {
            QTextTable *table = cursor.currentTable();
            if (!table && !cursor.block().text().startsWith(QStringLiteral(" "))) {
                QString number = QString::number(blockNumber + 1)+" ";
                painter.setPen(Qt::black);
                painter.drawText(commentIconWidth, top, lineNumberArea->width() - commentIconWidth, fontMetrics().height(), Qt::AlignRight, number);

                if (hasComment(blockNumber)) {
                    int commentIndicatorSize = fontMetrics().height() / 2;
                    QRect commentIndicatorRect(commentIconWidth / 2 - commentIndicatorSize / 2, top + commentIndicatorSize / 2, commentIndicatorSize, commentIndicatorSize);
                    painter.setBrush(Qt::black);
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(commentIndicatorRect);
                }

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
    int commentIconWidth = fontMetrics().height();
    return space + commentIconWidth;
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
    QTextCursor cursor = cursorForPosition(event->pos());
    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        QTextCharFormat charFormat = cursor.charFormat();
        if (charFormat.isAnchor()) {
            QUrl url(charFormat.anchorHref());
            emit linkClicked(url);
        }
    }
    QTextEdit::mousePressEvent(event);
}

void LineNumberTextEdit::addComment(int lineNumber, const QString& comment)
{
    comments.insert(lineNumber, comment);
    lineNumberArea->update();
}

void LineNumberTextEdit::removeComment(int lineNumber) {
    comments.remove(lineNumber);
    lineNumberArea->update();
}

bool LineNumberTextEdit::hasComment(int lineNumber) const
{
    return comments.contains(lineNumber);
}

void LineNumberTextEdit::showCommentDialog(int lineNumber)
{
    bool ok;
    QString comment = QInputDialog::getMultiLineText(this, tr("Comment"), tr("Enter your comment:"), comments.value(lineNumber), &ok);
    if (ok) {
        comments.insert(lineNumber, comment);
    }
}

void LineNumberTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    QTextCursor cursor = cursorForPosition(pos);
    int lineNumber = cursor.blockNumber();

    QMenu menu(this);
    const QIcon addCommentIcon = QIcon("./images/add-comment.png");
    const QIcon editCommentIcon = QIcon("./images/edit-comment.png");
    const QIcon showCommentIcon = QIcon("./images/show-comment.png");
    const QIcon removeCommentIcon = QIcon("./images/remove-comment.png");
    QAction *addAction = menu.addAction(addCommentIcon, tr("Add Comment"));
    QAction *editAction = menu.addAction(editCommentIcon, tr("Edit Comment"));
    QAction *showAction = menu.addAction(showCommentIcon, tr("Show Comment"));
    QAction *removeAction = menu.addAction(removeCommentIcon, tr("Remove Comment"));

    bool hasComment = comments.contains(lineNumber);
    editAction->setEnabled(hasComment);
    showAction->setEnabled(hasComment);
    removeAction->setEnabled(hasComment);
    addAction->setDisabled(hasComment);

    QAction *selectedAction = menu.exec(event->globalPos());
    if (selectedAction == addAction) {
        QString comment = QInputDialog::getText(this, tr("Add Comment"), tr("Comment:"), QLineEdit::Normal);
        if (!comment.isEmpty()) {
            addComment(lineNumber, comment);
        }
        emit commentChanged();
    } else if (selectedAction == editAction) {
        QString initialComment = comments.value(lineNumber);
        bool ok;
        QString comment = QInputDialog::getText(this, tr("Edit Comment"), tr("Comment:"), QLineEdit::Normal, initialComment, &ok);
        if (ok) {
            if (comment.isEmpty()) {
                removeComment(lineNumber);
            } else {
                addComment(lineNumber, comment);
            }
            emit commentChanged();
        }
    } else if (selectedAction == showAction) {
        emit showComment(comments[lineNumber]);
    } else if (selectedAction == removeAction) {
        removeComment(lineNumber);
        emit commentChanged();
    }

    lineNumberArea->update();
}

void LineNumberTextEdit::textChangedSlot()
{
    QString currentText = this->toPlainText();

    int previousLineCount = previousText.count('\n');
    int currentLineCount = currentText.count('\n');
    int lineOffset = currentLineCount - previousLineCount;

    QTextCursor cursor = this->textCursor();
    int position = cursor.position();
    int insertDeleteLine = this->document()->findBlock(position).blockNumber();

    QMap<int, QString> updatedComments;
    for (auto it = comments.begin(); it != comments.end(); ++it) {
        int oldLineNumber = it.key();
        QString comment = it.value();

        if (oldLineNumber > insertDeleteLine) {
            int newLineNumber = oldLineNumber + lineOffset;
            updatedComments.insert(newLineNumber, comment);
        } else {
            updatedComments.insert(oldLineNumber, comment);
        }
    }

    comments = updatedComments;
    update();
    previousText = currentText;
}

QString LineNumberTextEdit::getComment(int lineNumber) const
{
    return comments.value(lineNumber);
}
