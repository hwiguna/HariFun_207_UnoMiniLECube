class Atom
{
    public:
        Atom(int speed, int acceleration);
    bool Forward();
    bool Backward();
    byte* GetPreviousPosition();
    byte* GetCurrentPosition();
    private:
        int _speed;
    int _acceleration;
    int _previousIndex;
    int _currentIndex;
    unsigned long _timeToMove;
    const byte _path[12][3]= {
        {0,0,3},{0,1,3},{0,2,3},{0,3,3}, // left towards rear
        {1,3,3},{2,3,3},{3,3,3}, // rear towards right
        {3,2,3},{3,1,3},{3,0,3}, // right towards front
        {2,0,3},{1,0,3},
    };
};


Atom::Atom(int speed, int acceleration) {
    _speed = speed;
    _acceleration = acceleration;
    _previousIndex = 0;
    _currentIndex = 0;
    _timeToMove = millis() + _speed;
}

bool Atom::Forward() {
    if (millis() > _timeToMove) {
        _previousIndex = _currentIndex;
        _currentIndex++;
        if (_currentIndex>11) _currentIndex=0;
        _timeToMove = millis() + _speed;
        _speed += _acceleration;
        if (_speed <10 || _speed > 500) _acceleration = - _acceleration;
        return true;
    }
    else
        return false;
}

bool Atom::Backward() {
    if (millis() > _timeToMove) {
        _previousIndex = _currentIndex;
        _currentIndex--;
        if (_currentIndex<0) _currentIndex=11;
        _timeToMove = millis() + _speed;
        _speed += _acceleration;
        if (_speed <10 || _speed > 500) _acceleration = - _acceleration;
        return true;
    }
    else
        return false;
}

byte* Atom::GetPreviousPosition()
{
    return _path[_previousIndex];
}

byte* Atom::GetCurrentPosition()
{
    return _path[_currentIndex];
}
