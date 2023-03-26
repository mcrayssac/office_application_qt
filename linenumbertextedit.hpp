#ifndef LINENUMBERTEXTEDIT_HPP
#define LINENUMBERTEXTEDIT_HPP

#include <QTextEdit>
#include <QTextBlock>
#include <QMap>

class LineNumberArea;

class LineNumberTextEdit : public QTextEdit
{
Q_OBJECT

public:
    explicit LineNumberTextEdit(QWidget *parent = nullptr);
    /* Gérer la zone de numéro de ligne */
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setViewportMarginsPublic(int left, int top, int right, int bottom);
    /* Mettre à jour la zone de numéro de ligne */
    void updateLineNumberArea(const QRect &rect, int dy);

    /* Commentaires */
    void addComment(int lineNumber, const QString& comment);
    void removeComment(int lineNumber);
    bool hasComment(int lineNumber) const;
    QString getComment(int lineNumber) const;

protected:
    /* Gérer le redimensionnement des numéros si augmentation/reduction de la window */
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

public slots:
    void showCommentDialog(int lineNumber);
    void textChangedSlot();

private slots:
    /* Mettre à jour la largeur de la zone de numéro de ligne */
    void updateLineNumberAreaWidth();
    void onScrollBarValueChanged(int value);
    void cursorPositionChangedSlot();

private:
    /* Zone de numéro de ligne */
    LineNumberArea *lineNumberArea;
    /* Map pour les commentaires */
    QMap<int, QString> comments;
    QString previousText;

signals:
    void linkClicked(const QUrl &url);
    void showComment(const QString &comment);
    void commentChanged();
};


class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    LineNumberArea(LineNumberTextEdit *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

signals:
    void commentIconClicked(int lineNumber);

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            int lineNumber = 1;
            emit commentIconClicked(lineNumber);
        }
        QWidget::mousePressEvent(event);
    }

private:
    LineNumberTextEdit *codeEditor;
};

#endif
