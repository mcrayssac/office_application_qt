#include "window.hpp"
#include <QtWidgets>
#include <QPushButton>
#include <QSessionManager>
#include <iostream>
#include "hunspell/hunspell.hxx"
// Load dictionary for spelling, to be sure where it is located type hunspell -D
Hunspell spellChecker("/usr/share/hunspell/en_US.aff", "/usr/share/hunspell/en_US.dic");

MainWindow::MainWindow() : textEdit(new QPlainTextEdit), fontSize(14) {
    setCentralWidget(textEdit);

    createActions();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

    // Auto save
    autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);

    // Word count

    wordCountLabel = new QLabel(this);
    charCountLabel = new QLabel(this);
    lineCountLabel = new QLabel(this);

    statusBar()->addPermanentWidget(wordCountLabel);
    statusBar()->addPermanentWidget(charCountLabel);
    statusBar()->addPermanentWidget(lineCountLabel);

    connect(textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::updateCounts);

    updateCounts();

    QFont defaultFont = textEdit->font();
    defaultFont.setPointSize(fontSize);
    textEdit->setFont(defaultFont);

    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    format.setFont(QFont("Arial", fontSize));
    cursor.setCharFormat(format);
    textEdit->setTextCursor(cursor);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile() {
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save() {
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
    autoSaveTimer->start(30000); // Enable auto-save every 30 seconds
}


bool MainWindow::saveAs() {
    QFileDialog dialog(this, tr("Save As"));
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    QString fileName = dialog.selectedFiles().first();
    bool result = saveFile(fileName);

    if (result) {
        autoSaveTimer->start(30000); // Enable auto-save every 30 seconds
    }

    return result;
}

void MainWindow::autoSave() {
    if (!curFile.isEmpty()) {
        save();
        autoSaveTimer->start(30000);
    }
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates how to "
                          "write modern GUI applications using Qt, with a menu bar, "
                          "toolbars, and a status bar."));
}

void MainWindow::documentWasModified() {
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::writeSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::readSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

bool MainWindow::maybeSave() {
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    autoSave(); // Save the file immediately after opening
}

void MainWindow::setCurrentFile(const QString &fileName) {
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER

void MainWindow::commitData(QSessionManager &manager) {
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (textEdit->document()->isModified())
            save();
    }
}

#endif

bool MainWindow::saveFile(const QString &fileName)
//! [44] //! [45]
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(fileName),
                                          file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::bold() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    if (selectedFormat.fontWeight() == QFont::Bold) {
        format.setFontWeight(QFont::Normal);
    } else {
        format.setFontWeight(QFont::Bold);
    }

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::italic() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    if (selectedFormat.fontItalic()) {
        format.setFontItalic(false);
    } else {
        format.setFontItalic(true);
    }

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::underline() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    if (selectedFormat.fontUnderline()) {
        format.setFontUnderline(false);
    } else {
        format.setFontUnderline(true);
    }

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::superscript() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
        QTextCursor selectedCursor = cursor;
        selectedCursor.clearSelection();
        selectedCursor.movePosition(QTextCursor::NoMove, QTextCursor::MoveAnchor);
        selectedCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                                    cursor.selectionEnd() - cursor.selectionStart());
        selectedFormat = selectedCursor.charFormat();
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    if (selectedFormat.verticalAlignment() == QTextCharFormat::AlignSuperScript) {
        format.setVerticalAlignment(QTextCharFormat::AlignNormal);
        format.setFontPointSize(selectedFormat.fontPointSize());
    } else {
        format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        format.setFontPointSize(selectedFormat.fontPointSize() * 0.5);
    }

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::subscript() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
        QTextCursor selectedCursor = cursor;
        selectedCursor.clearSelection();
        selectedCursor.movePosition(QTextCursor::NoMove, QTextCursor::MoveAnchor);
        selectedCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                                    cursor.selectionEnd() - cursor.selectionStart());
        selectedFormat = selectedCursor.charFormat();
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    if (selectedFormat.verticalAlignment() == QTextCharFormat::AlignSubScript) {
        format.setVerticalAlignment(QTextCharFormat::AlignNormal);
        format.setFontPointSize(selectedFormat.fontPointSize());
    } else {
        format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
        format.setFontPointSize(selectedFormat.fontPointSize() * 0.5);
    }

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::increaseFontSize()
{
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
        double newPointSize = selectedFormat.fontPointSize() + 1;
        format.setFontPointSize(newPointSize);
        cursor.mergeCharFormat(format);
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    double newPointSize = selectedFormat.fontPointSize() + 1;
    format.setFontPointSize(newPointSize);

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::decreaseFontSize()
{
    QTextCursor cursor = textEdit->textCursor();
    QTextCharFormat format;
    QTextCharFormat selectedFormat;

    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
        QTextCursor selectedCursor = cursor;
        selectedCursor.clearSelection();
        selectedCursor.movePosition(QTextCursor::NoMove, QTextCursor::MoveAnchor);
        selectedCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, cursor.selectionEnd() - cursor.selectionStart());
        selectedFormat = selectedCursor.charFormat();
    } else {
        cursor.select(QTextCursor::WordUnderCursor);
        selectedFormat = cursor.charFormat();
        cursor.clearSelection();
    }

    double newPointSize = selectedFormat.fontPointSize() - 1;
    if (newPointSize < 1) {
        newPointSize = 1;
    }
    format.setFontPointSize(newPointSize);

    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::uppercase()
{
    QTextCharFormat format;
    QTextCharFormat selectedFormat;
    QString selectedText;

    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
        QTextCursor selectedCursor = cursor;
        selectedCursor.clearSelection();
        selectedCursor.movePosition(QTextCursor::NoMove, QTextCursor::MoveAnchor);
        selectedCursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        selectedFormat = selectedCursor.charFormat();
        selectedText = cursor.selectedText().toUpper();
        cursor.insertText(selectedText);
        cursor.clearSelection();
    } else {
        selectedText = cursor.block().text().mid(cursor.positionInBlock()).toUpper();
        cursor.insertText(selectedText, format);
        format.clearProperty(QTextFormat::FontCapitalization);
        format.setFontCapitalization(QFont::AllUppercase);
        textEdit->mergeCurrentCharFormat(format);
    }
}

void MainWindow::lowercase()
{
    QTextCharFormat format;
    QTextCharFormat selectedFormat;
    QString selectedText;

    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()) {
        selectedFormat = cursor.charFormat();
        QTextCursor selectedCursor = cursor;
        selectedCursor.clearSelection();
        selectedCursor.movePosition(QTextCursor::NoMove, QTextCursor::MoveAnchor);
        selectedCursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        selectedFormat = selectedCursor.charFormat();
        selectedText = cursor.selectedText().toLower();
        cursor.insertText(selectedText);
        cursor.clearSelection();
    } else {
        selectedText = cursor.block().text().mid(cursor.positionInBlock()).toLower();
        cursor.insertText(selectedText, format);
        format.clearProperty(QTextFormat::FontCapitalization);
        format.setFontCapitalization(QFont::AllLowercase);
        textEdit->mergeCurrentCharFormat(format);
    }
}

void MainWindow::updateCounts() {
    QString text = textEdit->toPlainText();
    int wordCount = text.split(QRegularExpression(R"((\s|\n|\r)+)"), Qt::SkipEmptyParts).count();
    int charCount = text.length();
    int lineCount = text.count('\n') + 1;

    wordCountLabel->setText(tr("Words: %1").arg(wordCount));
    charCountLabel->setText(tr("Characters: %1").arg(charCount));
    lineCountLabel->setText(tr("Lines: %1").arg(lineCount));
}
void MainWindow::searchAndReplace() {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Search and Replace"));

    QLabel searchLabel(tr("Search for:"));
    QLineEdit searchLineEdit;
    QLabel replaceLabel(tr("Replace with:"));
    QLineEdit replaceLineEdit;
    QPushButton okButton(tr("OK"));
    QPushButton cancelButton(tr("Cancel"));

    QGridLayout layout;
    layout.addWidget(&searchLabel, 0, 0);
    layout.addWidget(&searchLineEdit, 0, 1);
    layout.addWidget(&replaceLabel, 1, 0);
    layout.addWidget(&replaceLineEdit, 1, 1);
    layout.addWidget(&okButton, 2, 0);
    layout.addWidget(&cancelButton, 2, 1);
    dialog.setLayout(&layout);

    connect(&okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted) {
        QString search = searchLineEdit.text();
        QString replace = replaceLineEdit.text();

        QTextCursor cursor = textEdit->textCursor();
        qDebug() << "Search term: " << search;
        while (!cursor.isNull() && !cursor.atEnd()) {
            cursor = textEdit->document()->find(search, cursor, QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords);
            if (!cursor.isNull()) {
                QString foundText = cursor.selectedText();
                qDebug() << "Found search term at position " << cursor.position() << " with text: " << foundText;
                cursor.insertText(replace);
            }
        }
        if (cursor.atEnd()) {
            std::cout << "Search term not found" << std::endl;
        }
    }
}
QStringList MainWindow::getSpellingSuggestions(const QString &word) {
    QStringList suggestions;

    if (spellChecker.spell(word.toStdString())) {
        return suggestions;
    }

    std::vector<std::string> hunspellSuggestions = spellChecker.suggest(word.toStdString());
    for (const std::string &suggestion : hunspellSuggestions) {
        suggestions.append(QString::fromStdString(suggestion));
    }

    return suggestions;
}

void MainWindow::checkSpelling() {
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);

    while (!cursor.atEnd()) {
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selectedText();
        QStringList suggestions = getSpellingSuggestions(word);

        if (!suggestions.isEmpty()) {
            // Show suggestions in a custom context menu
            QMenu contextMenu;
            contextMenu.setTitle("Spelling Suggestions");

            for (const QString &suggestion : suggestions) {
                QAction *action = contextMenu.addAction(suggestion);
                action->setData(suggestion);
                connect(action, &QAction::triggered, this, &MainWindow::replaceMisspelledWordWithSuggestion);
            }

            // Show the context menu at the cursor position
            QPoint cursorPos = textEdit->mapToGlobal(textEdit->cursorRect().bottomRight());
            contextMenu.exec(cursorPos);
        }

        cursor.clearSelection();
        cursor.movePosition(QTextCursor::NextWord);
    }
}

void MainWindow::replaceMisspelledWordWithSuggestion() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action && !action->data().isNull()) {
        QString suggestion = action->data().toString();
        QTextCursor cursor = textEdit->textCursor();
        cursor.insertText(suggestion);
    }
}


void MainWindow::createActions() {

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
#ifndef QT_NO_CLIPBOARD
    // CUT
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    // COPY
    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    //  PASTE
    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

    QMenu *formatMenu = menuBar()->addMenu(tr("&Format"));
    QToolBar *formatToolBar = addToolBar(tr("Format"));

    const QIcon boldIcon = QIcon("./images/bold.png");
    QAction *boldAct = new QAction(boldIcon, tr("&Bold"), this);
    boldAct->setShortcut(QKeySequence::Bold);
    boldAct->setStatusTip(tr("Make the text bold"));
    connect(boldAct, &QAction::triggered, this, &MainWindow::bold);
    formatMenu->addAction(boldAct);
    formatToolBar->addAction(boldAct);

    const QIcon italicIcon = QIcon("./images/italic.png");
    QAction *italicAct = new QAction(italicIcon, tr("&Italic"), this);
    italicAct->setShortcut(QKeySequence::Italic);
    italicAct->setStatusTip(tr("Make the text italic"));
    connect(italicAct, &QAction::triggered, this, &MainWindow::italic);
    formatMenu->addAction(italicAct);
    formatToolBar->addAction(italicAct);

    const QIcon underlineIcon = QIcon("./images/underline.png");
    QAction *underlineAct = new QAction(underlineIcon, tr("&Underline"), this);
    underlineAct->setShortcut(QKeySequence::Underline);
    underlineAct->setStatusTip(tr("Make the text underline"));
    connect(underlineAct, &QAction::triggered, this, &MainWindow::underline);
    formatMenu->addAction(underlineAct);
    formatToolBar->addAction(underlineAct);

    const QIcon superscriptIcon = QIcon("./images/superscript.png");
    QAction *superscriptAct = new QAction(superscriptIcon, tr("&Superscript"), this);
    superscriptAct->setStatusTip(tr("Make the text superscript"));
    connect(superscriptAct, &QAction::triggered, this, &MainWindow::superscript);
    formatMenu->addAction(superscriptAct);
    formatToolBar->addAction(superscriptAct);

    const QIcon subscriptIcon = QIcon("./images/subscript.png");
    QAction *subscriptAct = new QAction(subscriptIcon, tr("&Subscript"), this);
    subscriptAct->setStatusTip(tr("Make the text subscript"));
    connect(subscriptAct, &QAction::triggered, this, &MainWindow::subscript);
    formatMenu->addAction(subscriptAct);
    formatToolBar->addAction(subscriptAct);

    const QIcon increaseFontSizeIcon = QIcon("./images/increaseFontSize.png");
    QAction *increaseFontSizeAct = new QAction(increaseFontSizeIcon, tr("&IncreaseFontSize"), this);
    increaseFontSizeAct->setStatusTip(tr("Increase font size"));
    connect(increaseFontSizeAct, &QAction::triggered, this, &MainWindow::increaseFontSize);
    formatMenu->addAction(increaseFontSizeAct);
    formatToolBar->addAction(increaseFontSizeAct);

    const QIcon decreaseFontSizeIcon = QIcon("./images/decreaseFontSize.png");
    QAction *decreaseFontSizeAct = new QAction(decreaseFontSizeIcon, tr("&DecreaseFontSize"), this);
    decreaseFontSizeAct->setStatusTip(tr("Decrease font size"));
    connect(decreaseFontSizeAct, &QAction::triggered, this, &MainWindow::decreaseFontSize);
    formatMenu->addAction(decreaseFontSizeAct);
    formatToolBar->addAction(decreaseFontSizeAct);

    const QIcon uppercaseIcon = QIcon("./images/uppercase.png");
    QAction *uppercaseAct = new QAction(uppercaseIcon, tr("Uppercase"), this);
    uppercaseAct->setStatusTip(tr("Convert selected text to uppercase"));
    connect(uppercaseAct, &QAction::triggered, this, &MainWindow::uppercase);
    formatMenu->addAction(uppercaseAct);
    formatToolBar->addAction(uppercaseAct);

    const QIcon lowercaseIcon = QIcon("./images/lowercase.png");
    QAction *lowercaseAct = new QAction(lowercaseIcon, tr("Lowercase"), this);
    lowercaseAct->setStatusTip(tr("Convert selected text to lowercase"));
    connect(lowercaseAct, &QAction::triggered, this, &MainWindow::lowercase);
    formatMenu->addAction(lowercaseAct);
    formatToolBar->addAction(lowercaseAct);

    // Add a separator above search and replace
    editMenu->addSeparator();

    // SEARCH AND REPLACE
    const QIcon searchAndReplaceIcon = QIcon("./images/searchAndReplace.png");
    QAction *searchAndReplaceAct = new QAction(searchAndReplaceIcon, tr("SearchAndReplace"), this);
    searchAndReplaceAct->setStatusTip(tr("Search and replace text"));
    connect(searchAndReplaceAct, &QAction::triggered, this, &MainWindow::searchAndReplace);
    editMenu->addAction(searchAndReplaceAct);
    editToolBar->addAction(searchAndReplaceAct);

    editMenu->addSeparator();

    // SPELLING
    const QIcon spellingIcon =  QIcon(":/images/spelling.png");
    QAction *checkSpellingAction = new QAction(spellingIcon, tr("Check Spelling"), this);
    checkSpellingAction->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    connect(checkSpellingAction, &QAction::triggered, this, &MainWindow::checkSpelling);
    // Add the action to a menu
    editMenu->addAction(checkSpellingAction);

#endif // !QT_NO_CLIPBOARD

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}