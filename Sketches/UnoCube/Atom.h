class Atom
{
    public:
        Atom(byte alongWhichAxis, byte offset, int speed, int lifeCycles);
    bool Forward();
    bool Backward();
    byte GetAxis();
    byte GetOffset();
    byte* GetPreviousPosition();
    byte* GetCurrentPosition();
    int Atom::GetLifeCycles();
    private:
        byte _alongWhichAxis;
        byte _offset;
        int _speed;
    int _lifeCycles;
    int _previousIndex;
    int _currentIndex = -1;
    unsigned long _timeToMove;
    const byte _path[12][3]= {
        {0,0,3},{0,1,3},{0,2,3},{0,3,3}, // left towards rear
        {1,3,3},{2,3,3},{3,3,3}, // rear towards right
        {3,2,3},{3,1,3},{3,0,3}, // right towards front
        {2,0,3},{1,0,3},
    };
};


Atom::Atom(byte alongWhichAxis, byte offset, int speed, int lifeCycles) {
    _alongWhichAxis = alongWhichAxis;
    _offset = offset;
    _speed = speed;
    _lifeCycles = lifeCycles;
    _previousIndex = 0;
    _currentIndex = 0;
    _timeToMove = millis() + _speed;
}

bool Atom::Forward() {
    if (_lifeCycles > 0 && millis() > _timeToMove) {
        _previousIndex = _currentIndex;
        _currentIndex++;
        if (_currentIndex>11) {_currentIndex=0; _lifeCycles--;}
        _timeToMove = millis() + _speed;
        return true;
    }
    else
        return false;
}

bool Atom::Backward() {
    if (_lifeCycles > 0 && millis() > _timeToMove) {
        _previousIndex = _currentIndex;
        _currentIndex--;
        if (_currentIndex<0) { _currentIndex=11; _lifeCycles--;}
        _timeToMove = millis() + _speed;
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

byte Atom::GetAxis()
{
    return _alongWhichAxis;
}

byte Atom::GetOffset()
{
    return _offset;
}

int Atom::GetLifeCycles()
{
    return _lifeCycles;
}
