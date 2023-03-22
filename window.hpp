#include <QMainWindow>
#include <QPlainTextEdit>
#include <QCloseEvent>
#include <QSessionManager>
#include <QLabel>
#include <QActionGroup>



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void loadFile(const QString &fileName);
    void undo();
    void redo();

protected:
    void closeEvent(QCloseEvent *event) override;
    int defaultFontSize;

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

    QActionGroup *themeActionGroup;
    QMenu *fileMenu;

    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString curFile;

    // Auto save
    QTimer *autoSaveTimer;
    // Word count
    QLabel *wordCountLabel;
    QLabel *charCountLabel;
    QLabel *lineCountLabel;

    QToolBar *fileToolBar;

    int fontSize;
};