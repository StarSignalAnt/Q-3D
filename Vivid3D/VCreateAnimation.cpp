#include "VCreateAnimation.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "Editor.h"
#include "qspinbox.h"

VCreateAnimation::VCreateAnimation(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	auto v = new QVBoxLayout();
	auto h = new QHBoxLayout();

	auto lab = new QLabel("Animation Name:");

	auto name = new QLineEdit();

	m_Name = name;

	h->addWidget(lab);
	h->addWidget(name);

	v->addLayout(h);

	auto buts = new QHBoxLayout();

	auto create = new QPushButton("Create");

	auto cancel = new QPushButton("Cancel");

	buts->addWidget(create);
	buts->addWidget(cancel);

	v->addLayout(buts);

	v->setAlignment(Qt::AlignTop);

	auto time = new QHBoxLayout();

	QLabel* time_lab = new QLabel("Length(Seconds)");
	QSpinBox* time_len = new QSpinBox(this);
	m_Len = time_len;

	time->addWidget(time_lab);
	time->addWidget(time_len);
	time_len->setMinimumWidth(90);
	time_len->setValue(10);

	time->setAlignment(Qt::AlignLeft);

	v->addLayout(time);

	setLayout(v);
	setFixedSize(450, 180);

	connect(create, &QPushButton::clicked, this, [&]() {
		//QMessageBox::information(this, tr("Button Clicked"), tr("The button was clicked!"));
		Editor::CreateAnimation(m_Name->text().toStdString(),m_Len->value());
		close();
		});

}

VCreateAnimation::~VCreateAnimation()
{}
