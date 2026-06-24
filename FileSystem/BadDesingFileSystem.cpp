#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
using namespace std;

class Entity{
    string name;
    bool isFile;
public:
    Entity(string name, bool isFile): name(name), isFile(isFile) {}
    string getName() {
        return name;
    }
    string getType() {
        return isFile ? "File" : "Folder";
    }

    string setName(string newName) {
        name = newName;
        return name;
    }

};

class Folder: public Entity {
    vector<Entity*> children;
    Folder(string name): Entity(name, false) {}
    public:
    void addChild(Entity* child) {
        children.push_back(child);
    }

    void removeChild(Entity* child) {
        children.erase(remove(children.begin(), children.end(), child), children.end());
    }

    void listChildren() {
        for (auto child : children) {
            cout << child->getName() << endl;
        }
    }

    void renameChild(Entity* child, string newName) {
        child->setName(newName);
    }

};

class File: public Entity {
    int size;
    File(string name,int size): Entity(name, true), size(size) {

    }
};


class InMemoryFileSystem {
    Folder* root;
    unordered_map<string, Entity*> pathMap;
    void splitPath(string path, string& parentPath, string& entityName) {
        size_t pos = path.find_last_of('/');
        if (pos == string::npos) {
            parentPath = "/";
            entityName = path;
        } else {
            parentPath = path.substr(0, pos);
            entityName = path.substr(pos + 1);
        }
    }
public:
    InMemoryFileSystem() {
        root = new Folder("/"); 
        pathMap["/"] = root;
    }

    vector<Entity*> list(string path) {
        if (pathMap.find(path) != pathMap.end()) {
            Folder* folder = dynamic_cast<Folder*>(pathMap[path]);
            if (folder) {
                return folder->listChildren();
            }
        }
        return {};
    }

    
   void createEntity(string path, bool isFile, int size = 0) {
        string parentPath, entityName;
        splitPath(path, parentPath, entityName);
        if (pathMap.find(parentPath) != pathMap.end()) {
            Folder* parentFolder = dynamic_cast<Folder*>(pathMap[parentPath]);
            if (parentFolder) {
                Entity* newEntity;
                if (isFile) {
                    newEntity = new File(entityName, size);
                } else {
                    newEntity = new Folder(entityName);
                }
                parentFolder->addChild(newEntity);
                pathMap[path] = newEntity;
            }
        }
    }

    void deleteEntity(string path) {
        if (pathMap.find(path) != pathMap.end()) {
            Entity* entity = pathMap[path];
            string parentPath, entityName;
            splitPath(path, parentPath, entityName);
            if (pathMap.find(parentPath) != pathMap.end()) {
                Folder* parentFolder = dynamic_cast<Folder*>(pathMap[parentPath]);
                if (parentFolder) {
                    parentFolder->removeChild(entity);
                    pathMap.erase(path);
                    delete entity;
                }
            }
        }
    }

    void renameEntity(string path, string newName) {
        if (pathMap.find(path) != pathMap.end()) {
            Entity* entity = pathMap[path];
            string parentPath, entityName;
            splitPath(path, parentPath, entityName);
            if (pathMap.find(parentPath) != pathMap.end()) {
                Folder* parentFolder = dynamic_cast<Folder*>(pathMap[parentPath]);
                if (parentFolder) {
                    parentFolder->renameChild(entity, newName);
                    string newPath = parentPath + "/" + newName;
                    pathMap.erase(path);
                    pathMap[newPath] = entity;
                }
            }
        }
    }
};
