#include <bits/stdc++.h>
using namespace std;
double distance(double lati1,double longti1,double lati2,double longti2) {
    double lat1 = lati1 * 3.14159265358979323846 / 180.0;
    double lon1 = longti1 * 3.14159265358979323846 / 180.0;
    double lat2 = lati2 * 3.14159265358979323846 / 180.0;
    double lon2 = longti2* 3.14159265358979323846 / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = pow(sin(dlat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon/2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    const double earth_radius = 6371.0; // bán kính Trái đất tính bằng km
    return earth_radius * c;
}
int main(){
    double longti[500];
    double lat[500];
    freopen("in.txt","r",stdin);
    freopen("out.txt","w",stdout);
    for(int i=0;i<=235;i++){
        cin>>lat[i]>>longti[i];
    }
    for(int i=0;i<=235;i++){
        for(int j=0;j<=235;j++){
            cout<<distance(lat[i],longti[i],lat[j],longti[j])<<" ";
        }
        cout<<endl;
    }
}