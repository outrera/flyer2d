// Copyright (C) 2008 Maciej Gajewski <maciej.gajewski0@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef FLYERGROUNDDECORATION_H
#define FLYERGROUNDDECORATION_H

#include <QRectF>
#include <QTransform>
#include <QList>

#include "texture.h"

#include "worldobject.h"

namespace Flyer
{

/**
It's a decorational world object responsible fro drawing ground surface (grass).
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class GroundDecoration : public WorldObject
{

public:
	GroundDecoration( World* pWorld );
	virtual ~GroundDecoration();

	// world object duties
	
	virtual QRectF boundingRect() const;
	virtual void render( QPainter& painter, const QRectF& rect, const RenderingOptions& options );
	
	// intialization
	
	void init( const QList<int>& textureIndices, const QRectF& boundingRect
		, const QTransform& transform, QList<Texture>* pTextures );

private:

	QRectF		_boundingRect;
	QTransform	_transform;
	QList<int>	_textureIndices;	///< Textrue indices
	QList<Texture>*	_pTextures;		///< Textures used to draw ground
};

}

#endif // FLYERGROUNDDECORATION_H

// EOF


