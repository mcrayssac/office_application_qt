# CREATING AN OFFICE APPLICATION WITH QT 📇
## WHAT IS THIS PROJECT? 

The Office Application is a desktop application built using Qt and C++ programming language. It is designed to provide similar functionality as LibreOffice. The application offers various features such as :

- Search and replace 🔎
- Spelling checker 🔤
- Word counter 🖨️
- Automatic saves 💾
- and multiple format tools ...

## INSTALLATION ⚙️:
 
- Download and install the Qt framework from https://www.qt.io/download

- Clone the project repository from https://github.com/mcrayssac/office_application_qt.git.

- Open the project in Qt Creator or CLion.

- Build and run the project.

## LIBRAIRIES 📖

This project use Hunspell. Hunspell is here used as a free spell checker and morphological analyzer library. It is designed for quick and high quality spell checking and correcting for languages with word-level writing system, including languages with rich morphology, complex word compounding and character encoding.

For more informations : https://github.com/hunspell/hunspell

For Ubuntu :
```
$ sudo apt install autoconf automake autopoint libtool
$ sudo apt-get install libhunspell-dev
```

Once installed, open the **window.cpp** file. You will find at line 7-8 this code.
<br> Type ```$ hunspell -D``` and verify your output. The output should be looking like this.

```
AVAILABLE DICTIONARIES (path is not mandatory for -d option):
/usr/share/hunspell/fr_MC
/usr/share/hunspell/fr_BE
/usr/share/hunspell/en_US
/usr/share/hunspell/fr_CA
/usr/share/hunspell/fr
/usr/share/hunspell/fr_LU
/usr/share/hunspell/fr_FR
/usr/share/hunspell/fr_CH
```

```
// Load dictionary for spelling, to be sure where it's located type hunspell -D
Hunspell spellChecker("/usr/share/hunspell/en_US.aff", "/usr/share/hunspell/en_US.dic");
```
