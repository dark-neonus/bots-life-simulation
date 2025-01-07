#pragma once

#include <stdexcept>
#include <climits>

class IDManager {
private:
    unsigned long idCounter = 0;
public:
    IDManager() {}

    /// @return Value that can be assigned as id for object. Auto incremented
    unsigned long getAssignValue() {
        if (idCounter == ULONG_MAX) {
            throw std::overflow_error("ID counter has reached its maximum value!");
        }
        idCounter++;
        return idCounter - 1;
    }

    /// @brief Return current value of idCounter. Dont use it in id assignments
    unsigned long getCurrentIdCounter() { return idCounter; }
};

class ObjectID {
private:
    bool idSet = false;
    unsigned long id;
public: 
    ObjectID() : idSet(false) {}
    ObjectID(unsigned long id_) : id(id_), idSet(true) {}

    /// @brief Set id to the given value. If id was already set, throw runtime_error
    void set(unsigned long id_) {
        if (!idSet) {
            id = id_;
        }
        else {
            throw std::runtime_error("ObjectID::id is unchangable!");
        }
    }
    /// @return ID value
    unsigned long get() { return id; }
};