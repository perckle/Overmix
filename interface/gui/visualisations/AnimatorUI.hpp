-/*
	This file is part of Overmix.

	Overmix is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Overmix is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Overmix.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ANIMATOR_UI_HPP
#define ANIMATOR_UI_HPP


#include <planes/ImageEx.hpp>

#include <QImage>
#include <QDialog>

class imageViewer;
class QSpinBox;

namespace Overmix{

struct DoubleSpinbox2D;
class AContainer;

class AnimatorUI : public QDialog{
	Q_OBJECT
	
	private:
		imageViewer* viewer;
		QImage img;
		DoubleSpinbox2D* skip;
		DoubleSpinbox2D* offset;
		
	public:
		AnimatorUI(QImage img, QWidget* parent );
		
	public slots:
		void update_preview();
};

}

#endif
