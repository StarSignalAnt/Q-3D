#pragma once

#include <QWidget>
#include "ui_VTextEditor.h"
#include <qtextedit.h>
#include "VCodeComplete.h"
#include <QString>
#include <QTextCursor>
class VTextEditor : public QTextEdit
{
	Q_OBJECT

public:
	VTextEditor(QWidget *parent = nullptr);
	~VTextEditor();
    bool searchBackwardAndReplace(const QString& searchStr, const QString& replaceStr)
    {
        if (searchStr.isEmpty() || replaceStr.isEmpty()) {
            return false;
        }

        QTextCursor cursor = textCursor();
        int initialPosition = cursor.position();

        // Start search from one character before the current position
        if (initialPosition > 0) {
            cursor.setPosition(initialPosition - 1);
        }
        else {
            return false; // At the start of the document, can't go backward
        }

        QString text = toPlainText();
        int searchLength = searchStr.length();
        int currentPos = cursor.position();

        while (currentPos >= searchLength - 1) {
            QString substring = text.mid(currentPos - searchLength + 1, searchLength);

            if (substring == searchStr) {
                // Found a match
                cursor.setPosition(currentPos - searchLength + 1);
                cursor.setPosition(currentPos + 1, QTextCursor::KeepAnchor);
                cursor.insertText(replaceStr);
                setTextCursor(cursor);
                return true;
            }

            currentPos--;
            cursor.setPosition(currentPos);
        }

        // If no match found, restore original cursor position
        cursor.setPosition(initialPosition);
        setTextCursor(cursor);
        return false;
    }

protected:
	void keyPressEvent(QKeyEvent* event) override {

		if (m_CodeComplete && m_CodeComplete->isVisible()) {
			switch (event->key()) {
			case Qt::Key_Up:
			case Qt::Key_Down:
		
				// Pass these keys to the completion widget
				QApplication::sendEvent(m_CodeComplete, event);
				
				return;
			case Qt::Key_Enter:
			case Qt::Key_Return:

				auto sel = m_CodeComplete->GetSelection();
				auto search = m_CodeComplete->GetSearch();

                searchBackwardAndReplace(search.c_str(), sel.c_str());
                m_CodeComplete->close();
                m_CodeComplete = nullptr;
                return;
				break;
			}
		}
		QTextEdit::keyPressEvent(event);


	}

public:
	VCodeComplete* GetCodeComplete() {
		return m_CodeComplete;
	}
	void SetCodeComplete(VCodeComplete* complete) {
		m_CodeComplete = complete;
	}

private:
	Ui::VTextEditorClass ui;
	VCodeComplete* m_CodeComplete = nullptr;
};
