#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H

#include<vector>
#include<QListWidgetItem>

using namespace std;

struct WindowState {
    bool showBoundingBox;
    vector<int> ISOSurfaces;
    int nextISOSurfaceID = 0;
    int curISOSurfaceID = -1;
    QListWidgetItem *curISOSurface=NULL;
    void setCurISOSurfaceItem(QListWidgetItem *item)
    {
        curISOSurface = item;
        curISOSurfaceID = -1;
        if(curISOSurface==NULL) return;
        // "ISO Surface " => len 12
        auto id = curISOSurface->text().toStdString().substr(12);
        curISOSurfaceID = stoi(id);
    }
    int getCurISOSurfaceID()
    {
        if(curISOSurface==NULL) return -1;
        return curISOSurfaceID;
    }
};

#endif // WINDOWSTATE_H
