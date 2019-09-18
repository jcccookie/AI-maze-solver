#include"player.h"
#include"actor.h"
#include"stack.h"
#include"list.h"
#include"point.h"
#include"aquarium.h"

#include<iostream>

//  update():	This is the only function you need to implement for player;
//				Everything else is already complete.
//
//
//		This function implements an algorithm to look through the maze
//      for places to move (STATE::LOOKING) using a queue. update also handles 
//		moving the player as well, which is not explicit in the slides.
//		If there is an open and undiscovered cell to look around the  player 
//		just moves there.  However, there will be cases where next desired point 
//		to look around is not adjacent (more than one cell away) to the player. 
//		At this point the player must backtrack (State::BACKTRACK) to a point 
//		that is adjacent to the next point the player was planning to visit
//
//		The player can only do one thing each call, they can either LOOK or 
//		BACKTRACK not both.  Nor should the player move more than one cell per
//		call to update.  If you examine the function calls for the entire game
//		you should observe that this is already be called within a loop. Your
//		Implementation should NOT have any loops that pertain to actual movement 
//		of the player.  
//
//		Backtracking is challenging, save it for the very very very last thing.
//		Make sure the Looking aspect compiles and works first.
void Player::update() {
	if (m_toggleBackTracking && sqrDist(this->getPosition(), this->getTargetPoint()) > 1) {

		this->setState(State::BACKTRACK);

		while (sqrDist(m_btBufferStack.peek(), getTargetPoint()) <= 1) {
			m_backTrack.push(m_btBufferStack.peek());
			setPosition(m_btBufferStack.peek());
			m_btBufferStack.pop();
		}
		while (sqrDist(m_btBufferQueue.peek(), getTargetPoint()) <= 1) {
			m_backTrack.push(m_btBufferQueue.peek());
			setPosition(m_btBufferQueue.peek());
			m_btBufferQueue.pop();
		}

		if (!m_backTrack.isEmpty()) {
			setPosition(m_backTrack.peek());
		}


		/*
		while (sqrDist(m_btBufferQueue.peek(), getTargetPoint()) <= 1) {
			m_backTrack.push(m_btBufferQueue.peek());
			m_btBufferStack.push(m_btBufferQueue.peek());
			m_btBufferQueue.pop();
		}
		while (sqrDist(m_btBufferStack.peek(), getTargetPoint()) <= 1) {
			m_backTrack.push(m_btBufferStack.peek());
			m_btBufferQueue.push(m_btBufferStack.peek());
			m_btBufferStack.pop();
		}
		if (!m_backTrack.isEmpty()) {
			setPosition(m_backTrack.peek());
		}
		*/
	}
	
	else {
		this->setState(State::LOOKING);
		//Get the position (first element in m_look)
		Point p = getTargetPoint();

		//If the queue is empty, NO Solution and done
		if (m_look.isEmpty()) {
			this->setState(State::STUCK);
			return;
		}

		//Set Nemo's position
		this->setPosition(p);

		if (m_toggleBackTracking) {
			//store nemo's preveious position to stack
			m_btBufferQueue.push(getPosition());
			m_btBufferStack.push(getPosition());
		}
		
		//If we're at the endpoint, done
		if (m_look.peek() == getAquarium()->getEndPoint()) {
			m_look.pop();
			this->setState(State::FREEDOM);
			return;
		}

		//Remove the top point from the queue
		m_look.pop();

		//x, y values
		int x = p.getX();
		int y = p.getY();

		//Check the directions and put the values to queue
		//Check west
		p.set(--x, y);
		if (getAquarium()->isCellOpen(p) && !_discovered(p)) {
			m_discovered.addToFront(p);
			m_look.push(p);
		}
		x++;
		//Check east
		p.set(++x, y);
		if (getAquarium()->isCellOpen(p) && !_discovered(p)) {
			m_discovered.addToFront(p);
			m_look.push(p);
		}
		x--;
		//Check north
		p.set(x, --y);
		if (getAquarium()->isCellOpen(p) && !_discovered(p)) {
			m_discovered.addToFront(p);
			m_look.push(p);
		}
		y++;
		//Check south
		p.set(x, ++y);
		if (getAquarium()->isCellOpen(p) && !_discovered(p)) {
			m_discovered.addToFront(p);
			m_look.push(p);
		}
	}
}



// No changes beyound this point

//  Player( ... )
//		Constructs and initializes the Player/Actor and its member variables
//		Remembers and discovers the starting point.
Player::Player(Aquarium* aquarium, Point p, std::string name, char sprite)
	:Actor(aquarium, p, name, sprite),
	m_look(),
	m_discovered(),
	m_backTrack(),
	m_btBufferQueue(),
	m_btBufferStack(),
	m_toggleBackTracking(false) {
	// Discover the starting point
	m_discovered.addToFront(p);
	m_look.push(p);
}

//  stuck() 
//		See if the player is stuck in the maze (no solution)
bool Player::stuck() const {
	return Actor::getState() == State::STUCK;
}

//  toggleBackTrack( ... )
//		Turn on/off backtracking
void Player::toggleBackTrack(bool toggle) {
	m_toggleBackTracking = toggle;
}
//  getTargetPoint()
//		Get the point the player wants to look around next.
Point Player::getTargetPoint() const {
	if (m_look.isEmpty()) return getAquarium()->getEndPoint();
	return m_look.peek();
}

// say()
//		What does the player say? depends on the players state and interactions
void Player::say() {

	// Freedom supercedes being eaten
	if (getState() == State::FREEDOM) {
		std::cout << getName() << ": WEEEEEEEEEEEEEEE!";
		return;
	} 
 
	// Being eaten supercedes being lost
	switch (getInteract()) {
	case Interact::ATTACK:
		std::cout << getName() << ": OUCH!";
		break;
	case Interact::GREET:
		break;
	case Interact::ALONE:
	default:
		switch (getState()) {
		case State::LOOKING:
			std::cout << getName() << ": Where's the exit?";
			break;
		case State::STUCK:
			std::cout << getName() << ": Oh no! I'm Trapped!";
			break;
		case State::BACKTRACK:
			std::cout << getName() << ": Gotta backtrack...";
			break;
		default:
			break;
		}

		break;
	}
}

// _discovered
//		returns true if the item is in the list
bool Player::_discovered(Point p) {
	return (m_discovered.findItem(p) != -1);
}
