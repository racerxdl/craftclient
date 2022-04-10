#pragma once

#include <vector>
#include <string>
#include <exception>
#include <memory>

typedef std::vector<unsigned char> Row;
typedef std::vector<Row> Rows;

class _Image {
public:
    int width, height;
    int bpp, colorType;
    Rows rows;
    Row &operator[](int n) { return rows[n]; }
    const Row operator[](int n) const { return rows[n]; }
};

class ImageException: public std::exception {
public:
    std::string msg;
    ImageException(const std::string &msg) : msg(msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

typedef std::shared_ptr<_Image> Image;

Image LoadPNG(std::string filename);