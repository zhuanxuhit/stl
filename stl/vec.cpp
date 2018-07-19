//
// Created by zhuanxu on 2018/6/8.
//

#include <vector>
#include <iostream>
#include <strings.h>

using namespace std;

int main() {
    vector<int> a;
    a.reserve(10);
    cout << a.size() << endl;

    {
        // size 固定
        vector<float> f(0.0);
        f.reserve(10);
        cout << f.size() << endl;
    }
    {
        int a = 1;
        int b = 10;
        float aa[a * b];
        bzero(aa, sizeof(aa));
        cout << sizeof(aa) << endl;
    }
    {
        int dimension = 16;
        int result_num = 2000;
        float video_features[dimension * result_num];
        bzero(video_features, sizeof(video_features));
        cout << (sizeof(video_features) == (dimension * result_num * sizeof(float))) << " " << video_features[121]
             << endl;
    }
}