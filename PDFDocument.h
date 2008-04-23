/*
    This is part of TeXworks, an environment for working with TeX documents
    Copyright (C) 2007-08  Jonathan Kew

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef PDFDocument_H
#define PDFDocument_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QList>
#include <QCursor>
#include <QButtonGroup>
#include <QPainterPath>

#include "poppler-qt4.h"
#include "synctex_parser.h"

#include "ui_PDFDocument.h"

const int kDefault_MagnifierSize = 2;
const bool kDefault_CircularMagnifier = true;

class QAction;
class QMenu;
class QToolBar;
class QScrollArea;
class TeXDocument;

class PDFMagnifier : public QLabel
{
	Q_OBJECT

public:
	PDFMagnifier(QWidget *parent, qreal inDpi);
	void setPage(Poppler::Page *p, qreal scale);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);

private:
	Poppler::Page	*page;
	qreal	scaleFactor;
	qreal	dpi;
	QImage	image;
	
	QPoint	imageLoc;
	QSize	imageSize;
	qreal	imageDpi;
	Poppler::Page	*imagePage;
};

typedef enum {
	kFixedMag,
	kFitWidth,
	kFitWindow
} autoScaleOption;

class PDFWidget : public QLabel
{
	Q_OBJECT

public:
	PDFWidget();
	void setDocument(Poppler::Document *doc);

	void saveState(); // used when toggling full screen mode
	void restoreState();
	void setResolution(int res);
	void resetMagnifier();
	void goToPage(int pageIndex);
	void setHighlightPath(const QPainterPath& path);

private slots:
	void goFirst();
	void goPrev();
	void goNext();
	void goLast();
	void doPageDialog();
	
	void actualSize();
	void fitWidth(bool checked);
	void zoomIn();
	void zoomOut();
	
public slots:
	void windowResized();
	void fitWindow(bool checked);
	void setTool(int tool);

signals:
	void changedPage(int);
	void changedZoom(qreal);
	void changedScaleOption(autoScaleOption);
	void syncClick(int, const QPointF&);

protected:
	virtual void paintEvent(QPaintEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

	virtual void focusInEvent(QFocusEvent *event);

private:
	void init();
	void reloadPage();
	void adjustSize();
	void updateStatusBar();
	void updateCursor();
	void updateCursor(const QPoint& pos);
	void useMagnifier(const QMouseEvent *inEvent);
	void doLink(const Poppler::Link *link);
	
	Poppler::Document	*document;
	Poppler::Page		*page;
	Poppler::Link		*clickedLink;

	int pageIndex;
	qreal	scaleFactor;
	qreal	dpi;
	autoScaleOption scaleOption;

	qreal			saveScaleFactor;
	autoScaleOption	saveScaleOption;
	
	QImage	image;
	QRect	imageRect;
	qreal	imageDpi;
	Poppler::Page	*imagePage;

	PDFMagnifier	*magnifier;
	int		currentTool;	// the current tool selected in the toolbar
	int		usingTool;	// the tool actually being used in an ongoing mouse drag

	QPainterPath	highlightPath;

	static QCursor	*magnifierCursor;
	static QCursor	*zoomInCursor;
	static QCursor	*zoomOutCursor;
};


class PDFDocument : public QMainWindow, private Ui::PDFDocument
{
	Q_OBJECT

public:
	PDFDocument(const QString &fileName, TeXDocument *sourceDoc = NULL);
	virtual ~PDFDocument();

	static PDFDocument *findDocument(const QString &fileName);
	static QList<PDFDocument*> documentList()
		{
			return docList;
		}

	QString fileName() const
		{ return curFile; }

	void zoomToRight(QWidget *otherWindow);
	void reload();
	void showScale(qreal scale);
	void showPage(int page);
	void setResolution(int res);
	void resetMagnifier();
	void enableTypesetAction(bool enabled);
	void updateTypesettingAction(bool processRunning);

protected:
	virtual void resizeEvent(QResizeEvent *event);

public slots:
	void selectWindow();

private slots:
	void updateRecentFileActions();
	void updateWindowMenu();
	void enablePageActions(int);
	void enableZoomActions(qreal);
	void adjustScaleActions(autoScaleOption);
	void retypeset();
	void interrupt();
	void goToSource();
	void toggleFullScreen();
	void syncClick(int page, const QPointF& pos);
	void syncFromSource(const QString& sourceFile, int lineNo);

signals:
	void windowResized();

private:
	void init();
	void loadFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	void loadSyncData();

	QString curFile;
	
	Poppler::Document	*document;
	
	PDFWidget	*pdfWidget;
	QScrollArea	*scrollArea;
	QButtonGroup	*toolButtonGroup;

	TeXDocument *sourceDoc;

	QLabel *pageLabel;
	QLabel *scaleLabel;
	QList<QAction*> recentFileActions;
	QMenu *menuRecent;

	synctex_scanner_t scanner;

	static QList<PDFDocument*> docList;
};

#endif
