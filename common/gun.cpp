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

#include "bullet.h"
#include "world.h"
#include "common.h"
#include "plane.h"
#include "b2dqt.h"
#include "body.h"

#include "gun.h"

namespace Flyer
{

static const double DAMAGED_INTERVAL	= 10.0;	///< Firing interval when damaged
static const double DAMAGED_VELOCITY	= 0.5;	///< Bullet velocity when damaged
static const double REACTION_MULTIPLIER = 0.1;	///< part of rection force whic hactually acts on shooting body

// ============================================================================
// Constructor
Gun::Gun ( Machine* pParent, const QString& name ) : System ( pParent, name )
{
	_firing = false;
	_timeFromLastFiring = 0;
	_broken = false;
	_muzzleShift = 0.0;
}

// ============================================================================
// Destructor
Gun::~Gun()
{
}

// ============================================================================
// Damages gun
void Gun::damage ( double force )
{
	if ( ! _broken )
	{
		double reduce = force / damageCapacity();
		
		switch ( qrand() % 2 )
		{
			// reduce fire rate
			case 0:
			{
				_currentnInterval += _interval * reduce * ( DAMAGED_INTERVAL - 1.0 );
				//qDebug("GUN: firing interval extended to %g from %g, force: %g", _currentnInterval, _interval, force );
				break;
			}
			// reduce velocity
			case 1:
			{
				_currentVelocity -= _velocity * reduce * ( 1.0 - DAMAGED_VELOCITY );
				//qDebug("GUN: Velocity reduced to %g form %g, force %g",  _currentVelocity, _velocity, force );
			}
			// TODO add deflection
		}
		
		// and maybe is totally borken?
		if ( ( qrand() % 1000 ) < ( reduce * 1000 ) )
		{
			_broken = true;
			//qDebug("Gun totally broken (chances where %g)", reduce);
		}
	}
	
}

// ============================================================================
// Smulates gun
void Gun::simulate( double dt )
{
	_timeFromLastFiring += dt;
	
	if ( body()->b2body() )
	{
		// it's time for firing?
		if ( ! _broken && _firing && ( _timeFromLastFiring > _currentnInterval ) )
		{
			b2Body* pBody = body()->b2body();
			
			// create bullet
			Bullet* pBullet = new Bullet( parent()->world() );			
			parent()->world()->addObject( pBullet, World::ObjectSimulated );
			
			
			pBullet->setMass( _mass );
			pBullet->setLifespan( _lifespan );
			pBullet->setSize( _size );
			pBullet->setRenderLayer( LayerForeground );
			
			// fire bullet
			b2Vec2 startPoint = pBody->GetWorldPoint( point2vec( _muzzle + _normal*_muzzleShift ) );
			b2Vec2 endPoint = pBody->GetWorldPoint( point2vec( _muzzle + _normal*(_muzzleShift+1) ) );
			
			b2Vec2 normal = endPoint - startPoint;
			
			QPointF velocity = vec2point( normal ) * _currentVelocity;
			
			pBullet->fire( vec2point( startPoint ), velocity );
			
			// apply reverse impulse to parent body
			//a = f/m;
			//v = a*t;
			
			//v = f*t/m
			//f = v*m/t
			double reaction = REACTION_MULTIPLIER * _velocity*_mass / dt;
			pBody->ApplyImpulse( -reaction*normal, startPoint );
			
			// reset timer
			_timeFromLastFiring = 0;
			
		}
		
	}
	
}

// ============================================================================
/// Estimates damage status, from 1.0 - fully operational to 0.1 - fully damaged.
double Gun::status() const
{
	if ( _broken )
	{
		return 0.0;
	}
	else
	{
		double intervalDamage = 1.0 - ( 1.0 - _interval / _currentnInterval ) / ( DAMAGED_INTERVAL - 1.0 );
		double velocityDamage = 1.0 - ( 1.0 - _currentVelocity / _velocity ) / ( 1.0 - DAMAGED_VELOCITY ); 
		
		return 0.5 * ( intervalDamage + velocityDamage );
	}
}

// ============================================================================
/// Repairs gun
void Gun::repair()
{
	_currentVelocity = _velocity;
	_currentnInterval = _interval;
	_broken = false;
}

// ============================================================================
// Creates kalashnikow subachine infantry gun.
Gun* Gun::kalashikov( Machine* pParent, const QString& name)
{
	Gun* pGun = new Gun( pParent, name );
	pGun->setBulletMass( 7.91E-3 ); 
	pGun->setBulletVelocity( 735 );
	pGun->setBulletSize( 7.85E-3 );
	pGun->setFiringInterval( 0.2 ); // otoginal: 600rpm, here: 300 rpm
	pGun->setBulletLifespan( 2.0 );
	pGun->setDamageCapacity( 100E3 );
	
	return pGun;
}

// ============================================================================
// Creates berezin aircraft machine gun.
Gun* Gun::berezin( Machine* pParent, const QString& name)
{
	Gun* pGun = new Gun( pParent, name );
	pGun->setBulletMass( 60E-3 );
	pGun->setBulletVelocity( 830 );
	pGun->setBulletSize( 12E-3 );
	pGun->setFiringInterval( 0.15 ); // original: 800 rounds per minute, here: 360
	pGun->setBulletLifespan( 3.0 );
	pGun->setDamageCapacity( 100E3 );
	
	return pGun;
}

// ============================================================================
/// Sets nortmal in world coordinates, independend of body rotation.
void Gun::setWorldNormal( const QPointF& n )
{
	b2Vec2 worldMuzzle = body()->b2body()->GetWorldPoint( point2vec( _muzzle ) );
	b2Vec2 worldEndpoint = worldMuzzle + point2vec( n );
	b2Vec2 localEndpoint = body()->b2body()->GetLocalPoint( worldEndpoint );
	_normal = vec2point( localEndpoint - point2vec( _muzzle ) );
}

}
