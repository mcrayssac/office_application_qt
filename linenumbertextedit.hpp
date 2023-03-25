#ifndef LINENUMBERTEXTEDIT_HPP
#define LINENUMBERTEXTEDIT_HPP

#include <QTextEdit>
#include <QTextBlock>

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

protected:
    /* Gérer le redimensionnement des numéros si augmentation/reduction de la window */
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    /* Mettre à jour la largeur de la zone de numéro de ligne */
    void updateLineNumberAreaWidth();
    void onScrollBarValueChanged(int value);
    void cursorPositionChangedSlot();

private:
    /* Zone de numéro de ligne */
    LineNumberArea *lineNumberArea;

signals:
    void linkClicked(const QUrl &url);
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(LineNumberTextEdit *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    LineNumberTextEdit *codeEditor;
};

#endif // LINENUMBERTEXTEDIT_HPP
