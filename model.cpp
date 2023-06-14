#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), texts_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) {
                iss >> v.raw[i];
            }
            verts_.push_back(v);
        } else if (!line.compare(0, 4, "vt  ")) {
            // We need to do iss >> trash twice because there are two characters
            // iss >> trash;
            // iss >> trash;
            
            // I believe you can also use a string
            std::string trashStr;
            iss >> trashStr;
            Vec3f vt;
            for (int i=0;i<3;i++) {
                iss >> vt.raw[i];
            }
            texts_.push_back(vt);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx, vt_idx;

            // Just so I don't forget
            // iss - input string stream
            // From the input string stream, take the first character from the string stream and put it into a variable trash
            iss >> trash;

            // >> idx - since this is an int, take the first integer and throw it into idx
            // >> trash - continue until you reach a character and throw it into trash
            // >> vt_idx - continue until you reach an integer and throw it into vt_idx
            // and so on...
            while (iss >> idx >> trash >> vt_idx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                vt_idx--;
                
                // This means we need to offset by one to get the vertex coordinates or texture coordinates
                // We'll now have a length of 6 instead of 3 now
                f.push_back(idx);
                f.push_back(vt_idx);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << texts_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::ntexts() {
    return (int)texts_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::text(int i) {
    return texts_[i];
}
