#include "VScriptEdit.h"
#include "qsplitter.h"
#include <QTextDocument>
#include <QTextEdit>
#include <QTextCursor>
#include <QListWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QPoint>
#include <QRect>
#include <qabstracttextdocumentlayout.h>
#include <qtextbrowser.h>
#include <QTextBlock>
#include <QTextDocument>
#include "VVarGroup.h"
#include "ScriptHost.h"
#include <unordered_set>
#include "Editor.h"
#include "VCodeBody.h"

#include <algorithm>
VScriptEdit::VScriptEdit(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    //setBaseSize(700, 700);
    resize(800, 700);

    setWindowTitle("Vivid3D - V Script Editor");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Create menu bar
    QMenuBar* menuBar = new QMenuBar(this);

    // Create file menu and add actions
    QMenu* fileMenu = menuBar->addMenu("File");
    QAction* saveAction = fileMenu->addAction("Save");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("Exit");

    // Create edit menu and add actions
    QMenu* editMenu = menuBar->addMenu("Edit");
    QAction* cutAction = editMenu->addAction("Cut");
    QAction* copyAction = editMenu->addAction("Copy");
    QAction* pasteAction = editMenu->addAction("Paste");

    // Create toolbar
    QToolBar* toolBar = new QToolBar(this);
    toolBar->addAction(saveAction);
    toolBar->addSeparator();
  //  toolBar->addAction(cutAction);
  //  toolBar->addAction(copyAction);
    //toolBar->addAction(pasteAction);
    QObject::connect(exitAction, &QAction::triggered, [=]() {

        close();
        Editor::m_ScriptEdit = nullptr;

        });

    QObject::connect(saveAction, &QAction::triggered, [=]() {
        SaveScript();
        //        QMessageBox::information(mainWindow, "Action Triggered", "The action was triggered!");
        // You can put any code you want here
        // For example:
        // mainWindow->someFunction();
        // int result = someCalculation();
        // updateUI(result);
        });

    QIcon icon("engine/v6.png");

    // Set the window icon
    setWindowIcon(icon);

    // Add menu bar and toolbar to the layout
    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(toolBar);

    QSplitter* sp = new QSplitter(Qt::Orientation::Vertical);

    // Create a central widget (e.g., QTextEdit)
   // VTextEditor* textEdit = new VTextEditor(this);
   // mainLayout->addWidget(textEdit);
    m_Console = new QTextEdit(this);
    //mainLayout->addWidget(m_Console);
    QPalette p = m_Console->palette();
    p.setColor(QPalette::Base, QColor(40, 40, 40));
    m_Console->setPalette(p);

    m_Docs = new QTabWidget(this);

   
    sp->addWidget(m_Docs);
    sp->addWidget(m_Console);
    mainLayout->addWidget(sp);
    QList<int> sizes;
    sizes << 500 << 200;  // Set the desired sizes for the top and bottom widgets
    sp->setSizes(sizes);

 //   m_Console->setMaximumHeight(250);
    setLayout(mainLayout);
    m_This = this;
   // m_Edit = textEdit;
    m_MainTimer = new QTimer(this);

    connect(m_MainTimer, &QTimer::timeout, this, &VScriptEdit::on_Timer);

    m_MainTimer->setInterval(1000);
    m_MainTimer->start();


    //Offer AI enabled responses to compile problems/errors.

    m_Codes["class"] = KeyColor(42, 104, 189);
    m_Codes["func"] = KeyColor(209, 151, 187);
    m_Codes["end"] = KeyColor(209, 151, 187);
    m_Codes["module"] = KeyColor(200, 0, 0);
    m_Codes["if"] =  KeyColor(209, 151, 187);
    m_Codes["else"] = KeyColor(209, 151, 187);
    m_Codes["elseif"] = KeyColor(209, 151, 187);
    m_Codes["switch"] = KeyColor(209, 151, 187);
    m_Codes["break"] = KeyColor(209, 151, 187);
    m_Codes["return"] = KeyColor(209, 151, 187);
    m_Codes["for"] = KeyColor(209, 151, 187);
    m_Codes["int"] = KeyColor(42, 104, 189);
    m_Codes["float"] = KeyColor(42, 104, 189);
    m_Codes["string"] = KeyColor(42, 104, 189);
    m_Codes["enum"] = KeyColor(42, 104, 189);

  
    Editor::m_ScriptEdit = this;

 
 


}

bool saveQTextEditToFile(QTextEdit* textEdit, const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "Error", "Could not open file for writing: " + file.errorString());
        return false;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();

    file.close();

    if (file.error() != QFile::NoError)
    {
        QMessageBox::critical(nullptr, "Error", "Error writing to file: " + file.errorString());
        return false;
    }

    return true;
}

std::string get_filename(const std::string& path) {
    // Use std::filesystem::path to handle the file path
    std::filesystem::path p(path);
    // Return the filename with extension
    return p.filename().string();
}

void VScriptEdit::LoadScript(std::string path) {

    

//    setWindowTitle(title.c_str());

    m_Edit = new VTextEditor;

    QPalette p = m_Edit->palette();
    p.setColor(QPalette::Base,QColor(30,30,30));
    m_Edit->setPalette(p);


    m_Edits.push_back(m_Edit);

    m_Docs->addTab((QWidget*)m_Edit, get_filename(path).c_str());

    QFile file(path.c_str());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_Edit->setPlainText(in.readAll());
        file.close();
    }
    m_Path = path;

    std::string filen = get_filename(path);

    std::string title = "Vivid3D - Script Editor - " + filen;
    m_Edit->setTabStopDistance(10);



    // Connect the textChanged signal of the QTextEdit to the onTextChanged slot
    connect(m_Edit, &QTextEdit::textChanged, this, &VScriptEdit::onCodeChanged);
    QObject::connect(m_Edit, &QTextEdit::cursorPositionChanged, [this]() {

        UpdateCodeComplete();

        });

    m_Docs->setCurrentIndex(m_Docs->count() - 1);

    m_CodeChanged = true;

}

void VScriptEdit::SaveScript()
{
    
    saveQTextEditToFile(m_Edit, m_Path.c_str());


}

VScriptEdit* VScriptEdit::m_This = nullptr;

VScriptEdit::~VScriptEdit()
{}


std::string VScriptEdit::getText()
{
    // Get the content of the QTextEdit as a QString
    QString qText = m_Edit->toPlainText();

    // Convert QString to std::string
    std::string stdText = qText.toStdString();

    return stdText;
}

void VScriptEdit::on_Timer() {

    if (!m_CodeChanged) return;
    m_CodeChanged = false;

    m_Console->clear();
    auto code = getText();

    int tab = m_Docs->currentIndex();

    m_Edit = m_Edits[tab];

    VSource* src = new VSource();
    src->SetSource(code);
    VTokenizer* toker = new VTokenizer();

    auto tokes = toker->Tokenize(src);

    VParser* parser = new VParser;

    parser->SetOutput(VScriptEdit::compile_Output);

    auto mod = parser->ParseModule(tokes);
      
    if (mod != nullptr) {
        for (auto c : mod->GetClasses()) {

            auto sub_cls = c->GetSubClass();

            if (sub_cls != "") {
                //
                for (auto sm : ScriptHost::m_This->GetContext()->GetModules()) {

                    for (auto sc : sm->GetClasses()) {

                        if (sc->GetName().GetNames()[0] == sub_cls) {

                            for (auto v : sc->GetGroups()) {
                                c->AddVarGroup(v);
                            }
                            for (auto vf : sc->GetFunctions()) {
                                c->AddFunction(vf);
                            }

                        }

                    }

                }
            }

        }
    }

    int b = 5;

    if (mod == nullptr) return;
    auto mods = ScriptHost::m_This->GetContext()->GetModules();
    for (auto m : mods) {

        for (auto c : m->GetClasses())
        {
            mod->AddClass(c);
        }
    }
    m_ClassList.clear();
    for (auto c : mod->GetClasses()) {

        m_ClassList.push_back(c->GetName().GetNames()[0]);
        for (auto v : c->GetGroups()) {

            for (auto vv : v->GetNames()) {

                m_VarList.push_back(vv.GetNames()[0]);

            }

        }
        for (auto f : c->GetFunctions()) {
            m_FuncList.push_back(f->GetName().GetNames()[0]);
            if(f->GetCode()!=nullptr){
                for (auto code : f->GetCode()->GetFor()) {

                    if (dynamic_cast<VVarGroup*>(code)) {

                        auto g = (VVarGroup*)code;
                        for (auto gv : g->GetNames())
                        {

                            m_VarList.push_back(gv.GetNames()[0]);

                        }

                    }
                }
            }
        }

    }

    if (mod == nullptr) {

    }else
    if (m_CodeModule == nullptr) {
        m_CodeModule = mod;

    }else
    if (mod->GetClasses().size() < m_CodeModule->GetClasses().size()) {

    }
    else {
        m_CodeModule = mod;
    }
 //   m_CodeChanged = false;

    UpdateCodeComplete();

    auto word = GetCurrentWord();

    auto words = GetCurrentWords();

    AppendConsole("Current Word:" + word);
    if (word.size() > 0) {
        AppendConsole("Words:" + words[0]);
        if (words.size() > 1)
        {
            AppendConsole("Word 2:" + words[1]);
        }
    }

    Highlight();


}

std::vector< std::string > VScriptEdit::GetCurrentWords() {
    std::vector<std::string> words;
    std::string currentWord = GetCurrentWord();

    // If the current word is empty, return an empty vector
    if (currentWord.empty()) {
        return words;
    }

    // Find the position of the dot
    size_t dotPos = currentWord.find('.');

    // If there's no dot, return a vector with just the current word
    if (dotPos == std::string::npos) {
        words.push_back(currentWord);
        return words;
    }

    // If there's a dot, split the word into two parts
    std::string firstPart = currentWord.substr(0, dotPos);
    std::string secondPart = currentWord.substr(dotPos + 1);

    // Add both parts to the vector
    words.push_back(firstPart);
    words.push_back(secondPart);

    return words;
}

std::string VScriptEdit::GetCurrentWord() {

    QTextCursor cursor = m_Edit->textCursor();

    // Get the current line
    cursor.select(QTextCursor::LineUnderCursor);
    QString line = cursor.selectedText();

    // Get the cursor position within the line
    int cursorPosInLine = cursor.positionInBlock();

    // Find the start of the word
    int start = cursorPosInLine;
    while (start > 0 && (line[start - 1].isLetterOrNumber() || line[start - 1] == '.' || line[start - 1] == '_')) {
        start--;
    }

    // Find the end of the word
    int end = cursorPosInLine;
    while (end < line.length() && (line[end].isLetterOrNumber() || line[end] == '.' || line[end] == '_')) {
        end++;
    }

    // Extract the word
    QString word = line.mid(start, end - start);

    // Convert to std::string and return
    return word.toStdString();

}


std::vector<std::string> removeDuplicates(std::vector<std::string>& vec) {
    std::unordered_set<std::string> seen;
    auto it = std::remove_if(vec.begin(), vec.end(),
        [&seen](const std::string& str) {
            if (seen.find(str) != seen.end()) {
                return true;  // Remove this element
            }
            seen.insert(str);
            return false;  // Keep this element
        });

    vec.erase(it, vec.end());
    return vec;
}

void VScriptEdit::UpdateCodeComplete() {

    m_ComList.clear();
  

    if (m_CodeModule == nullptr) return;
    if (m_CodeModule->GetClasses().size() == 0) return;


    /*
    for (auto cls : m_CodeModule->GetClasses()) {

        if (cls == nullptr) continue;
        m_ComList.push_back(cls->GetName().GetNames()[0]);
        m_ClassList.push_back(cls->GetName().GetNames()[0]);
        for (auto func : cls->GetFunctions()) {
            if (func->GetName().GetNames().size() == 0) {
                return;
            }
            m_ComList.push_back(func->GetName().GetNames()[0].c_str());
            m_FuncList.push_back(func->GetName().GetNames()[0].c_str());
        }
        for (auto v : cls->GetGroups()) {
            for (auto av : v->GetNames()) {

                m_ComList.push_back(av.GetNames()[0].c_str());

            }
        }


    }

    for (auto m : ScriptHost::m_This->GetContext()->GetModules()) {

        for (auto cls : m->GetClasses()) {

            if (cls == nullptr) continue;
            m_ComList.push_back(cls->GetName().GetNames()[0]);
            m_ClassList.push_back(cls->GetName().GetNames()[0]);
            for (auto func : cls->GetFunctions()) {
                m_ComList.push_back(func->GetName().GetNames()[0].c_str());
                m_FuncList.push_back(func->GetName().GetNames()[0].c_str());
            }
            for (auto v : cls->GetGroups()) {
                for (auto av : v->GetNames()) {

                    m_ComList.push_back(av.GetNames()[0].c_str());

                }
            }


        }
    }

    */

    std::string search = "";

    auto words = GetCurrentWords();

    auto cp = getCharacterPosition(m_Edit);

    AppendConsole("CP:" + std::to_string(cp));

    if (words.size() > 1) {

        auto cls = FindClass(words[0]);
        if (cls != nullptr) {
            AppendConsole("Class:" + cls->GetName().GetNames()[0]);

            for (auto v : cls->GetGroups()) {
                for (auto n : v->GetNames())
                {

                    m_ComList.push_back(n.GetNames()[0]);

                }
            }

            for (auto f : cls->GetFunctions()) {
                m_ComList.push_back(f->GetName().GetNames()[0]);
            }

        }
        search = words[1];
    }
    else if (words.size() == 1)
    {


        for (auto c : m_CodeModule->GetClasses()) {

            if (cp >= c->GetStart() && cp <= c->GetEnd()) {

                AppendConsole("In Class:" + c->GetName().GetNames()[0]);

                for (auto g : c->GetGroups()) {

                    for (auto n : g->GetNames()) {

                        m_ComList.push_back(n.GetNames()[0]);

                    }

                }

                for (auto f : c->GetFunctions()) {

                    m_ComList.push_back(f->GetName().GetNames()[0]);

                }

            }

        }

        search = words[0];
        /*
        for (auto cls : m_CodeModule->GetClasses()) {

            if (cls == nullptr) continue;
            m_ComList.push_back(cls->GetName().GetNames()[0]);
            m_ClassList.push_back(cls->GetName().GetNames()[0]);
            for (auto func : cls->GetFunctions()) {
                if (func->GetName().GetNames().size() == 0) {
                    return;
                }
                m_ComList.push_back(func->GetName().GetNames()[0].c_str());
                m_FuncList.push_back(func->GetName().GetNames()[0].c_str());
            }
            for (auto v : cls->GetGroups()) {
                for (auto av : v->GetNames()) {

                    m_ComList.push_back(av.GetNames()[0].c_str());

                }
            }


        }

        for (auto m : ScriptHost::m_This->GetContext()->GetModules()) {

            for (auto cls : m->GetClasses()) {

                if (cls == nullptr) continue;
                m_ComList.push_back(cls->GetName().GetNames()[0]);
                m_ClassList.push_back(cls->GetName().GetNames()[0]);
                for (auto func : cls->GetFunctions()) {
                    m_ComList.push_back(func->GetName().GetNames()[0].c_str());
                    m_FuncList.push_back(func->GetName().GetNames()[0].c_str());
                }
                for (auto v : cls->GetGroups()) {
                    for (auto av : v->GetNames()) {

                        m_ComList.push_back(av.GetNames()[0].c_str());

                    }
                }


            }
        }
        */
    }


    m_ComList.push_back("class");
    m_ComList.push_back("if");
    m_ComList.push_back("else");
    m_ComList.push_back("end");
    m_ComList.push_back("class");
    m_ComList.push_back("elseif");
    m_ComList.push_back("func");
    m_ComList.push_back("return");
    m_ComList.push_back("lambda");
    m_ComList.push_back("switch");
    m_ComList.push_back("break");
    m_ComList.push_back("for");
    m_ComList.push_back("static");


    m_ComList= removeDuplicates(m_ComList);


    auto word = GetCurrentWord();
    
    if (word.size() > 0) {

        if (m_Edit->GetCodeComplete() != nullptr) {
            m_Edit->GetCodeComplete()->close();
            m_Edit->SetCodeComplete(nullptr);
        }
        QTextCursor cursor = m_Edit->textCursor();
        QTextDocument* document = m_Edit->document();
        QPointF cursorPos = m_Edit->viewport()->mapToGlobal(document->documentLayout()->blockBoundingRect(cursor.block()).topLeft());

        // Create and configure the code completion widget
        VCodeComplete* completeWidget = new VCodeComplete();
        //completeWidget->setSuggestions(suggestions);
        m_Edit->SetCodeComplete(completeWidget);

        // Set the widget position to the cursor position
        QRect cursorRect = m_Edit->cursorRect(cursor);
        QPoint widgetPos = m_Edit->viewport()->mapToGlobal(cursorRect.bottomLeft());
        widgetPos.setX(widgetPos.x() + 32);
        widgetPos.setY(widgetPos.y() - 64);
        completeWidget->move(widgetPos);
        completeWidget->SetWord(search);


        for (auto item : m_ComList) {

            completeWidget->AddItem(item);

        }
        if (completeWidget->HasWord(word)) {

        }
        else {
            if (completeWidget->GetCount() == 0) {

            }
            else {

                completeWidget->SetWord(search);
                // Show the widget
                completeWidget->show();
            }
        }
    }
    else {
        if (m_Edit->GetCodeComplete() != nullptr) {
            //m_CodeComplete->close();
            m_Edit->GetCodeComplete()->close();
            m_Edit->SetCodeComplete(nullptr);
            //m_CodeComplete = nullptr;

        }
    }

}

VClass* VScriptEdit::FindClass(std::string name) {

    auto cls = m_CodeModule->FindClassVar(name);
    if (cls != nullptr) {
        return cls;
    }

    
    

    int b = 5;

  

    return nullptr;



}
void resetHighlighting(QTextDocument* document) {
    QTextCursor cursor(document);

    // Select the entire document
    cursor.select(QTextCursor::Document);

    // Create a default text format (e.g., black color text)
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(QBrush(Qt::white)); // Set to default text color

    // Apply the default format to the entire document
    cursor.setCharFormat(defaultFormat);
}
void VScriptEdit::Highlight() {

    QTextDocument* document = m_Edit->document();
    QTextCursor cursor(document);

    resetHighlighting(document);

    // Create a format for highlighting
    QTextCharFormat highlightFormat;
    //highlightFormat.setBackground(color);

       for (auto c : m_ClassList) {


        if (m_Codes.count(c) == 0) {
            m_Codes[c] = KeyColor(10, 170, 10);
        }
    }

    for (auto f : m_FuncList)
    {

        if (m_Codes.count(f) == 0) {
            m_Codes[f] = KeyColor(220, 170, 0);
        }

    }

    for (auto f : m_VarList)
    {

        if (m_Codes.count(f) == 0) {
            m_Codes[f] = KeyColor(120, 170, 170);
        }

    }


    for (const auto& pair : m_Codes) {
        const std::string& word = pair.first;
        const KeyColor& color = pair.second;

        // Create a format for highlighting
        QTextCharFormat textFormat;
        textFormat.setForeground(QColor(color.R, color.G, color.B));

        // Create a regular expression with word boundaries
        QRegularExpression regex(QString("\\b%1\\b").arg(QString::fromStdString(word)));

        // Start searching from the beginning of the document
        QTextCursor cursor(document);
        cursor.movePosition(QTextCursor::Start);

        while (!cursor.isNull() && !cursor.atEnd()) {
            cursor = document->find(regex, cursor);
            if (!cursor.isNull()) {
                // Apply the text color format
                cursor.mergeCharFormat(textFormat);
                cursor.movePosition(QTextCursor::NextCharacter); // Move past the found word
            }
        }
    }
    m_CodeChanged = false;

}

void VScriptEdit::onCodeChanged()
{
    m_CodeChanged = true;
    // This function will be called every time the text in the QTextEdit changes
    //QString currentText = textEdit->toPlainText();
    //qDebug() << "Text changed to:" << currentText;
}

void VScriptEdit::compile_Output(const std::string& s1, const std::string& s2) {
    //std::cout << "compile_Output called with: " << s1 << " and " << s2 << std::endl;
    m_This->AppendConsole("Error:" + s1 + " Type:" + s2);
}

void VScriptEdit::AppendConsole(std::string msg) {

    m_Console->append(msg.c_str());

}