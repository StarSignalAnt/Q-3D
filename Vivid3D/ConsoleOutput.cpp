#include "ConsoleOutput.h"

ConsoleOutput* ConsoleOutput::m_Instance = nullptr;

ConsoleOutput::ConsoleOutput(QWidget *parent)
	: QTextEdit(parent)
{
	//ui.setupUi(this);
    
    m_Instance = this;
    setStyleSheet(R"(
    QTextEdit {
        background-color: white;
        color: black;
    }
)");
}

ConsoleOutput::~ConsoleOutput()
{}

void ConsoleOutput::appendConsoleText(const QString& text, const QColor& color)
{
    QTextCharFormat format;
    format.setForeground(color);

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(format);
    cursor.insertText(text);
    setTextCursor(cursor);
    ensureCursorVisible();
}

void ConsoleOutput::appendConsoleLine(const QString& text, const QColor& color)
{
    appendConsoleText(text + "\n", color);
}