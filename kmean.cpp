#include<iostream>
#include<vector>
#include<string>
#include<fstream>
using namespace std;
vector<string> split(const string& s, const string& seperator) {
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size()) {
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while (i != s.size() && flag == 0) {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x]) {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0) {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x]) {
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ++j;
        }
        if (i != j) {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}

class Record
{
public:
    float* data;
    int len;
    Record(int l=14) {
        len = l;
        data = new float[len];
        for (size_t i = 0; i < len; i++)
        {
            data[i] = 0;
        }
    }
    Record& operator=(const Record& obj) {
        if (len != obj.len) {
            data = new float[obj.len];
            len = obj.len;
        }
        for (size_t i = 0; i < len; i++)
        {
            data[i] = obj.data[i];
        }
        return *this;
    }
    Record(const Record& obj) {
        if (len != obj.len) {
            data = new float[obj.len];
            len = obj.len;
        }
        for (size_t i = 0; i < len; i++)
        {
            data[i] = obj.data[i];
        }
    }
    void add(Record &r) {
        for (size_t i = 0; i < len; i++)
        {
            data[i] += r.data[i];
        }
    }
    void div(int num) {
        for (size_t i = 0; i < len; i++)
        {
            data[i] /= num;
        }
    }
    ~Record() { delete data; };
    Record(vector<string>& arr)
    {
        len = arr.size();
        data = new float[arr.size()];
        for (size_t i = 0; i < len; i++)
        {
            data[i] = stof(arr.at(i));
        }
    }
    friend ostream& operator<<(ostream& out, Record& r) {
        for (size_t i = 0; i < r.len; i++)
        {
            out << r.data[i]<<",";
        }
        out << endl;
        return out;
    }
};


int randIntRange(int min, int max, time_t* seed = 0)
{
    // 设置随机数范围
    int x = rand() % (max - min) + min;
    return x;
}

float distanceEuclidean(Record &r1, Record &r2)
{
    float sum = 0;
    for (size_t i = 0; i < r1.len; i++)
    {
        sum += (r1.data[i] - r2.data[i]) * (r1.data[i] - r2.data[i]);
    }
    return std::sqrt(sum);
}


void clusterMinDis(std::vector<Record>& src, int k, std::vector<Record>& center_pts, std::vector<std::vector<Record>>& dst)
{
    // 设置为k个簇集
    dst.resize(k);

    // 遍历所有其他点
    // 重置k个中心簇集
    for (int i = 0; i < k; i++)
    {
        std::vector<Record> temp;
        dst[i].swap(temp);
    }

    for (int j = 0; j < src.size(); j++)
    {
        // 计算其他点到各个中心点距离，进入最近中心点的簇集中
        int min_dis_center_idx = 0;
        float min_dis = 9999;
        for (int center_idx = 0; center_idx < k; center_idx++)
        {
            Record cur_center_pt = center_pts[center_idx];
            float distance = distanceEuclidean(src[j], center_pts[center_idx]);
            // 距离判断
            if (distance < min_dis)
            {
                min_dis = distance;
                min_dis_center_idx = center_idx;
            }
        }
        // 放入中心簇集
        dst[min_dis_center_idx].push_back(src[j]);
    }
    return;
}
void copy(Record& r1, Record& r2) {
    for (size_t i = 0; i < r1.len; i++)
    {
        r1.data[i] = r2.data[i];
    }
}
void kmeans(std::vector<Record>& src, int k, float min_dis_thresh, std::vector<Record>& center_pts, std::vector<std::vector<Record>>& dst)
{
    int dim = src[0].len;
    int pt_num = src.size();

    // 中心点数量 > 点集数量
    if (k > pt_num) return;

    // 初始化设置前k个点为中心点
    center_pts.clear();
    for (int i = 0; i < k; i++)
    {
        center_pts.push_back(src[i]);
    }

    float max_center_change_dist = 9999;

    // 簇集中最大的中心点距离变化值 > 阈值：需要更新簇集中心
    while (max_center_change_dist > min_dis_thresh)
    {

        max_center_change_dist = 0;
        clusterMinDis(src, k, center_pts, dst);

        // 计算所有簇集平均中心
        
        for (int i = 0; i < k; i++)
        {
            std::cout << "Cluster pt number:" << dst[i].size() << std::endl;
            Record center(dim);

            // 簇集无元素，跳过
            if (dst[i].size() == 0)
            {
                continue;
            }

            for (int j = 0; j < dst[i].size(); j++)
            {
                center.add(dst[i][j]);
            }
            center.div(dst[i].size());
            
            // 如果均值中心点距离原始中心点>设定值，更新中心点
            float cur_center_change_dist = distanceEuclidean(center, center_pts[i]);

            // 比较得到最大的中心点距离变化值
            if (cur_center_change_dist > max_center_change_dist)
            {
                max_center_change_dist = cur_center_change_dist;
            }
            std::cout << "更新簇集中心" << std::endl;
            std::cout << "中心点变化距离：" << max_center_change_dist << "，阈值：" << min_dis_thresh << std::endl;
            cout << center;
            copy(center_pts[i], center);
        }
    }
}


//void showResult(cv::Mat& res, std::vector<Point>& centerPoints, std::vector<std::vector<Point>>& clusterPoints, std::vector<cv::Scalar>& colorList)
//{
//    std::cout << "=======================" << std::endl;
//    for (int i = 0; i < clusterPoints.size(); i++)
//    {
//        std::cout << "Center point" << std::endl;
//        std::cout << centerPoints[i].x << "," << centerPoints[i].y << std::endl;
//        cv::circle(res, cv::Point(centerPoints[i].x, centerPoints[i].y), 6, colorList[i], 1);
//
//        for (int j = 0; j < clusterPoints[i].size(); j++)
//        {
//            std::cout << "Cluster point" << std::endl;
//            std::cout << clusterPoints[i][j].x << "," << clusterPoints[i][j].y << std::endl;
//            cv::circle(res, cv::Point(clusterPoints[i][j].x, clusterPoints[i][j].y), 4, colorList[i], -1);
//        }
//        std::cout << "=======================" << std::endl;
//    }
//    cv::imshow("K-means", res);
//    cv::waitKey(0);
//};

// 生成K-means例子
void generate_kmeans_example(int& clusterK, float& disThresh)
{
    std::vector<Record> centerRecords;
    std::vector<std::vector<Record>> clusterRecords;
    std::vector<Record> records;
 
    ifstream  in("normalization_data.txt");
    string input;
    if (!in.is_open())
    {
        cout << "未成功打开文件" << endl;
    }
    while (getline(in, input))
    {
        vector<string>strs = split(input, ",");
        Record record(strs);
        records.push_back(record);
    }
    in.close();

    // Inference
    kmeans(records, clusterK, disThresh, centerRecords, clusterRecords);

    // Show cluster results
    cout << "end";
    ofstream output("answer.txt");
    for (size_t i = 0; i < clusterRecords.size(); i++)
    {
        output << "cluster " << (i + 1) << endl;
        for (size_t j = 0; j < clusterRecords[i].size(); j++)
        {
            output << clusterRecords[i][j];
        }
        output << endl;
    }
    output.close();
    //showResult(res, centerPoints, clusterPoints, colorList);
    return;
}
int main()
{
    int clusterK = 4;
    float disThresh = 0.5;
    generate_kmeans_example(clusterK, disThresh);
    return 0;
}