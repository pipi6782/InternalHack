#pragma once

#define SafeDelete(p) {if(p != nullptr) delete p; p=nullptr;}
#define SafeDeleteArray(p) {if(p != nullptr) delete[] p; p=nullptr;}