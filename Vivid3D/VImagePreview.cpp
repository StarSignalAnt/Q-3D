#include "VImagePreview.h"
#include "TextureCube.h"

std::string getFileExtension(const std::string& filePath) {
	size_t lastDot = filePath.find_last_of(".");
	if (lastDot != std::string::npos) {
		return filePath.substr(lastDot);
	}
	return ""; // Return empty string if no extension found
}


VImagePreview::VImagePreview(QWidget *parent)
	: QLabel(parent)
{
	ui.setupUi(this);
	setStyleSheet("border: 2px dashed #aaa;");
	setAcceptDrops(true);
}

void VImagePreview::SetSize(int w, int h) {

	m_PrevW = w;
	m_PrevH = h;

}

void VImagePreview::SetImage(std::string path) {


	std::string ext = getFileExtension(path);

	if (ext == ".cm") {

		TextureCube* cube = new TextureCube(path);
		auto face = cube->GetFace(0);
		QPixmap *pix = new QPixmap(cube->GetWidth(), cube->GetHeight());
	


		QImage image(cube->GetWidth(), cube->GetHeight(), QImage::Format_ARGB32);

		// Access the pixel data in the QImage
		QRgb* imageData = reinterpret_cast<QRgb*>(image.bits());

		float* floatData = face->Data;

		// Convert float4 data to QImage format (ARGB)
		int idx = 0;
		for (int y = 0; y <cube->GetHeight(); ++y) {
			for (int x = 0; x <cube->GetWidth(); ++x) {
				float r = floatData[idx++];
				float g = floatData[idx++];
				float b = floatData[idx++];
				float a = floatData[idx++];

				// Convert float values (0.0 to 1.0) to integer (0 to 255)
				int ir = static_cast<int>(r * 255.0f);
				int ig = static_cast<int>(g * 255.0f);
				int ib = static_cast<int>(b * 255.0f);
				int ia = static_cast<int>(a * 255.0f);

				// Pack RGBA values into a QRgb format
				QRgb rgba = qRgba(ir, ig, ib, ia);

				// Set pixel in the QImage
				imageData[y * cube->GetWidth() + x] = rgba;
			}
		}


		QPixmap pixmap = QPixmap::fromImage(image);
		pixmap = pixmap.scaled(m_PrevW, m_PrevH);
		setPixmap(pixmap);
	}
	else {
		QPixmap map = QPixmap(path.c_str());
		map = map.scaled(m_PrevW, m_PrevH);
		setPixmap(map);
	}

}

void VImagePreview::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();

	}
}

void VImagePreview::dropEvent(QDropEvent* event) {
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urls = event->mimeData()->urls();
		QString filePath;
		if (!urls.isEmpty() && urls.first().isLocalFile()) {
			filePath = urls.first().toLocalFile();
			//setPixmap(QPixmap(filePath));
			SetImage(filePath.toStdString());
			emit dropped(filePath); // Emit the custom dropped sig
		}
		event->acceptProposedAction();
	}
}



VImagePreview::~VImagePreview()
{}
