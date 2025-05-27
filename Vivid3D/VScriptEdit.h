#pragma once

#include <QWidget>
#include "ui_VScriptEdit.h"
#include <QMenuBar>
#include <QMenu>
#include <qfile.h>
#include <qtabwidget.h>
#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextEdit>
#include <VContext.h>
#include "VVar.h"
#include "VModule.h"
#include "VParser.h"
#include "VClass.h"
#include "VFunction.h"
#include "VTokenizer.h"
#include "qtimer.h"
#include <map>
#include "VCodeComplete.h"
#include "VTextEditor.h"
#include <unordered_set>
#include <algorithm>
#include <thread>
#include <chrono>
struct KeyColor {
	int R, G, B;
	KeyColor(int r, int g, int b) {
		R = r;
		G = g;
		B = b;
	}
	KeyColor()
	{

	}
};


class VScriptEdit : public QWidget
{
	Q_OBJECT

public:
	VScriptEdit(QWidget *parent = nullptr);
	~VScriptEdit();
	void LoadScript(std::string path);
		
	static VScriptEdit* m_This;
	using c_out = std::function<void(const std::string&, const std::string&)>;

	std::string getText();
	std::string GetCurrentWord();
	std::vector<std::string> GetCurrentWords();
	void AppendConsole(std::string text);

	static void compile_Output(const std::string& t, const std::string& msg);
	void Highlight();
	void UpdateCodeComplete();
	VClass* FindClass(std::string name);
	int getCharacterPosition(QTextEdit* textEdit)
	{
		QTextCursor cursor = textEdit->textCursor();
		return cursor.position();
	}
	void SaveScript();
protected:
	

private slots:
	void on_Timer();

	void onCodeChanged();
private:
	Ui::VScriptEditClass ui;
	VTextEditor* m_Edit;
	QTextEdit* m_Console;
	QTimer* m_MainTimer;
	VModule* m_CodeModule;
	bool m_CodeChanged = true;
	c_out mOut;
	std::map<std::string, KeyColor> m_Codes;
	std::vector<std::string> m_ComList;
	std::vector<std::string> m_ClassList;
	std::vector<std::string> m_FuncList;
	std::vector<std::string> m_VarList;
	std::string m_Path;
	QTabWidget* m_Docs;
	std::vector<VTextEditor*> m_Edits;
};

