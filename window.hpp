#include <QMainWindow>
#include <QTextEdit>
#include <QCloseEvent>
#include <QSessionManager>
#include <QLabel>
#include <QActionGroup>
#include "linenumbertextedit.hpp"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void loadFile(const QString &fileName);
    void undo();
    void redo();
    int lineNumberAreaWidth();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void bold();
    void italic();
    void underline();
    void superscript();
    void subscript();
    void autoSave();
    void increaseFontSize();
    void decreaseFontSize();
    void uppercase();
    void lowercase();
    void replaceMisspelledWordWithSuggestion();
    static QStringList getSpellingSuggestions(const QString &word);
    void checkSpelling();
    void changeTheme(int index);
    void showThemeMenu();
    void searchReplaceFunction(const QString &search, const QString &replace, bool findWholeWords);
    void searchAndReplace();
    void onScrollBarValueChanged();
    void setColorSelectedText(const QColor &color);
    void setFontText(const QFont &font);
    void setFontSize(int size);
    void insertImage();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateCounts();
    void updateLineNumberAreaWidth();
    void highlightCurrentLine();
    void createZoomInAndZoomOut();

    QAction *zoomInAction;
    QAction *zoomOutAction;

    QActionGroup *themeActionGroup;
    QMenu *fileMenu;

    QString strippedName(const QString &fullFileName);

    QString curFile;

    // Auto save
    QTimer *autoSaveTimer;
    // Word count
    QLabel *wordCountLabel;
    QLabel *charCountLabel;
    QLabel *lineCountLabel;

    LineNumberTextEdit *textEdit; // Change QTextEdit to LineNumberTextEdit
    QWidget *lineNumberArea; // Add the lineNumberArea QWidget pointer

    int fontSize;
};