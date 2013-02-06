/*
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


#include "MultiImageIterator.h"

#include <cmath>


MultiImageIterator::MultiImageIterator( const std::vector<QImage> &images, const std::vector<QPoint> &points, int x, int y )
	:	imgs( images), pos( points ){
	values.reserve( imgs.size() );
	line_width.reserve( imgs.size() );
	lines.reserve( imgs.size() );
	new_y( y );
	new_x( x );
	
	//Find left_most point
	left = 99999;
	for( unsigned i=0; i<pos.size(); i++ )
		left = ( left > pos[i].x() ) ? pos[i].x() : left;
}


void MultiImageIterator::new_x( int x ){
	int offset = x - current_x;
	current_x = x;
	
	for( unsigned i=0; i<lines.size(); i++ )
		lines[i] += offset;
	
	values.clear();
}


void MultiImageIterator::new_y( int y ){
	//Check cache
	if( y == current_y && lines.size() != 0 )
		return;
	
	line_width.clear();
	lines.clear();
	
	for( unsigned iy=0; iy<imgs.size(); iy++ ){
		QPoint p = pos[iy];
		int new_y = y - p.y();
		if( new_y >= 0 && new_y < imgs[iy].height() ){
			lines.push_back( (const QRgb*)imgs[iy].constScanLine( new_y ) - p.x() + current_x );
			line_width.push_back( Line( p.x(), p.x() + imgs[iy].width() ) );
		}
	}
	
	current_y = y;
	values.clear();
}

void MultiImageIterator::fill_values(){
	if( values.size() == 0 )
		for( unsigned i=0; i<lines.size(); i++ )
			if( (current_x >= line_width[i].first) && (current_x < line_width[i].second) )
				values.push_back( color( *(lines[i]) ) );
}

color MultiImageIterator::average(){
	color avg;
	fill_values();
	
	if( values.size() ){
		for( unsigned i=0; i<values.size(); i++ )
			avg += values[i];
		avg /= values.size();
	}
	
	return avg;
}

color MultiImageIterator::difference(){
	color avg = average();
	color diff_avg;
	
	if( values.size() ){
		for( unsigned i=0; i<values.size(); i++ ){
			color diff( avg );
			diff.diff( values[i] );
			diff_avg += diff;
		}
		diff_avg /= values.size();
		diff_avg.r = std::sqrt( diff_avg.r ) * 256;
		diff_avg.g = std::sqrt( diff_avg.g ) * 256;
		diff_avg.b = std::sqrt( diff_avg.b ) * 256;
	}
	
	diff_avg.a = 255*256;
	return diff_avg;
}

color MultiImageIterator::simple_filter( unsigned threshould ){
	color avg = average();
	
	//Calculate value
	color r;
	unsigned amount = 0;
	for( unsigned i=0; i<values.size(); i++ ){
		//Find difference from average
		color d( values[i] );
		d.diff( avg );
		
		//Find the largest difference
		unsigned max = d.r > d.g ? d.r : d.g;
		max = d.b > max ? d.b : max;
		
		//Only apply if below threshould
		if( max <= threshould ){
			r += values[i];
			amount++;
		}
	}
	
	//Let it be transparent if no amount
	if( amount ){
		r.a *= amount;
		return r / amount;
	}
	else
		return color( 0,0,255*256 );
}

color MultiImageIterator::simple_slide( unsigned threshould ){
	fill_values();
	
	unsigned best = 0;
	color best_color;
	for( unsigned i=0; i<values.size(); i++ ){
		unsigned amount = 0;
		color avg;
		
		for( unsigned j=0; j<values.size(); j++ ){
			color d( values[i] );
			d.diff( values[j] );
			
			unsigned max = d.r > d.g ? d.r : d.g;
			max = d.b > max ? d.b : max;
			if( max <= threshould ){
				amount++;
				avg += values[j];
			}
		}
		
		if( amount > best ){
			best = amount;
			best_color = avg / amount;
		}
	}
	
	
	if( best ){
	//	if( do_diff )
	//		best_color.r = best_color.g = best_color.b = 255*256 * best / pixels.size();
		return best_color;
	}
	else
		return color( 0,0,255*256 );
}


struct color_comp{
	color c;
	unsigned gray;
	
	color_comp( color c ){
		this->c = c;
		gray = c.gray();
	}
	
	bool operator<( const color_comp& comp ) const{
		return gray < comp.gray;
	}
};

color MultiImageIterator::fast_slide( unsigned threshould ){
	fill_values();
	
	//Prepare array, avoid reallocs
	static std::vector<color_comp> comps;
	comps.clear();
	comps.reserve( imgs.size() );
	
	//Fill array with gray values stored
	for( unsigned i=0; i<values.size(); i++ )
		comps.push_back( color_comp( values[i] ) );
	
	//Sort it
	std::sort( comps.begin(), comps.end() );
	
	//Find the part where the window gives most samples
	unsigned best_amount = 0;
	unsigned best_index = 0;
	unsigned current_start = 0;
	unsigned current_width = 0;
	unsigned gray_sum = 0;
	for( int i=0; i<comps.size(); i++ ){
		
	}
}
