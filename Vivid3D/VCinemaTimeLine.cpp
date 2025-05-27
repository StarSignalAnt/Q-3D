#include "VCinemaTimeLine.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qframe.h>
#include <qevent.h>
#include "VTimeTrack.h"
#include "qmimedata.h"
#include "VNodeFrame.h"
#include "VTrackFrame.h"
#include "Editor.h"
#include "VCreateAnimation.h"

VCinemaTimeLine::VCinemaTimeLine(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_LO = new QVBoxLayout();

	m_LO->setAlignment(Qt::AlignTop);
	m_LO->setSpacing(5);

	

	auto top = new QHBoxLayout();
	auto additional_top = new QHBoxLayout();

	auto record_frame = new QPushButton("Record");

	additional_top->addWidget(record_frame);

	additional_top->setAlignment(Qt::AlignLeft);

	auto menuBar = new QMenuBar(this);

	QMenu* anim_Menu = new QMenu("Animation", this);



	auto new_anim = anim_Menu->addAction("New Animation");
	anim_Menu->addSeparator();
	anim_Menu->addAction("Load Animation");
	anim_Menu->addAction("Save Animation");


	connect(new_anim, &QAction::triggered, this, [this]() {
	//	QMessageBox::information(this, tr("Action Triggered"), tr("Example Action was triggered"));
		auto create_Anim = new VCreateAnimation;
		create_Anim->show();

		//Editor::CreateAnimation(anim_Name);

		});


	menuBar->addMenu(anim_Menu);

	m_LO->setMenuBar(menuBar);

	top->setAlignment(Qt::AlignLeft);
	top->setSpacing(0);

	QPushButton* to_Begin = new QPushButton("|<<");
	QPushButton* rewind = new QPushButton("<<");
	QPushButton* play = new QPushButton(">");
	QPushButton * ff = new QPushButton(">>");
	QPushButton* to_End = new QPushButton(">>|");

	to_Begin->setMaximumWidth(64);
	rewind->setMaximumWidth(64);
	play->setMaximumWidth(64);
	ff->setMaximumWidth(64);
	to_End->setMaximumWidth(64);

	top->addWidget(to_Begin);
	top->addWidget(rewind);
	top->addWidget(play);
	top->addWidget(ff);
	top->addWidget(to_End);

	QLabel* anim_NameLab = new QLabel("Animation");
	QLineEdit* anim_Name = new QLineEdit();

	top->addSpacing(32);
	top->addWidget(anim_NameLab);
	top->addSpacing(5);
	top->addWidget(anim_Name);
	anim_Name->setMaximumWidth(256);
	anim_NameLab->setMaximumWidth(80);

	VTrackFrame* track_panel = new VTrackFrame(this);
	track_panel->setMinimumWidth(600);
	track_panel->setFrameShape(QFrame::StyledPanel);
	track_panel->setStyleSheet("QFrame { background-color: #1c1c1c; }");


	VNodeFrame* node_panel = new VNodeFrame(this);

	node_panel->setMinimumWidth(320);
	node_panel->setMaximumWidth(320);
	node_panel->setMinimumHeight(320);
	node_panel->setMaximumHeight(1200);

	node_panel->setAcceptDrops(true);
	/*
	QObject::connect(node_panel, &QFrame::dragEnterEvent, [&node_panel](QDragEnterEvent* event) {
		if (event->mimeData()->hasUrls()) {
			event->acceptProposedAction();
		}
		});

	QObject::connect(node_panel, &QFrame::dropEvent, [&node_panel](QDropEvent* event) {
		const QMimeData* mimeData = event->mimeData();
		
			event->acceptProposedAction();

		});
	*/
	node_panel->setFrameShape(QFrame::StyledPanel);
	node_panel->setStyleSheet("QFrame { background-color: #1c1c1c; }");

	auto tracks = new QVBoxLayout();


	
	auto tracks_h = new QHBoxLayout();
	
	tracks_h->addWidget(node_panel);
	tracks_h->addWidget(track_panel);
	tracks_h->setAlignment(Qt::AlignTop);

	tracks->addLayout(tracks_h);

	auto node_LO = new QVBoxLayout();

	node_LO->addSpacing(32);

	node_panel->setLayout(node_LO);
	node_panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Ensure it can expand
	//node_LO->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Ensure it can expand



	
	node_LO->setAlignment(Qt::AlignTop);



	auto track_Line = new VTimeTrack(this);

	auto track_panel_LO = new QVBoxLayout();

	track_panel_LO->addWidget(track_Line);
	track_Line->setMaximumHeight(28);
	track_Line->setMinimumHeight(28);
	track_panel_LO->setAlignment(Qt::AlignTop);

	

	track_panel->setLayout(track_panel_LO);


	auto track_LO = new QVBoxLayout();

	tracks->setAlignment(Qt::AlignTop);

	node_panel->setLayout(track_LO);

	m_LO->addLayout(top);
	m_LO->addLayout(additional_top);
	m_LO->addLayout(tracks);

	setLayout(m_LO);
	
	m_TrackLO = track_panel_LO;
	m_NodeLO = node_LO;
	Editor::m_TrackPanel = track_panel;
	

}

VCinemaTimeLine::~VCinemaTimeLine()
{}
