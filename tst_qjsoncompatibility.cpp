#include <QtTest>


#include <QDir>
#include <QJsonDocument>

#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

#include "jjson17.h"

class QJsonCompatibility : public QObject
{
    Q_OBJECT

public:
    QJsonCompatibility();
    ~QJsonCompatibility();

private slots:
    void validation_self_test();
    void test_1_latin();
    void test_2_reals();
    void test_3_integers();
    void test_4_mix_latin_nums();
    void test_5_unicode_1();
    void test_6_escapeseq_1();
    void test_7_incaps_structures_1();
    void test_8_to_string();
    void test_9_implicit_cast();
    void perf_1_dirscan();
    void perf_2_dirscan();


    void parse_1_numbers();
    void parse_2_numbers_self();
    void parse_3_latin();
    void parse_3_latin_self();
    void parse_4_mix_latin_nums();
private:
    const QString scopeDirPath{"qjson"};

};

QJsonCompatibility::QJsonCompatibility()
{

}

QJsonCompatibility::~QJsonCompatibility()
{

}

#define READ_FILE_BACK() \
    QFile infile(filepath);                     \
    QVERIFY(infile.exists());                   \
    QVERIFY(infile.open(QIODevice::ReadOnly));  \
    QByteArray allData = infile.readAll();      \
    QVERIFY(!allData.isEmpty());                \
    infile.close();                             \
    auto doc = QJsonDocument::fromJson(allData);\
    QVERIFY(!doc.isNull())

void QJsonCompatibility::validation_self_test()
{
    //QSKIP("ALREADY VALIDATED");

    QString dirpath = scopeDirPath+"/validation_self_test";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);

    QJsonDocument doc;
    QString record_Name      = "TestName";
    QString record_Info      = "some short string 1234567";
    int     record_IntValue  = 33;
    double  record_RealValue = 389.75;
    QString record_UTF       = "기술적 설명";
    QString record_Escapes   = u8"AA\u2211BB\nCC\"XX\r99\fjjj\t(*)";

    QJsonObject obj({
                        {"record_Name"     ,record_Name     },
                        {"record_Info"     ,record_Info     },
                        {"record_IntValue" ,record_IntValue },
                        {"record_RealValue",record_RealValue},
                        {"record_UTF"      ,record_UTF      },
                        {"record_Escapes"  ,record_Escapes  }
                    });
    doc.setObject(obj);

    QFile outfile(dirpath+"/test.json");
          outfile.open(QIODevice::WriteOnly);
          outfile.write(doc.toJson());
          outfile.close();

    QFile infile(outfile.fileName());
    QVERIFY(infile.exists());
    QVERIFY(infile.open(QIODevice::ReadOnly));
    QByteArray allData = infile.readAll();
    QVERIFY(!allData.isEmpty());
    infile.close();

    doc = QJsonDocument::fromJson(allData);
    QVERIFY(!doc.isNull());
    obj = doc.object();
    QVERIFY(!obj.isEmpty());
    QCOMPARE(obj["record_Name"     ].toString(),record_Name     );
    QCOMPARE(obj["record_Info"     ].toString(),record_Info     );
    QCOMPARE(obj["record_IntValue" ].toInt   (),record_IntValue );
    QCOMPARE(obj["record_RealValue"].toDouble(),record_RealValue);
    QCOMPARE(obj["record_Escapes"  ].toString(),record_Escapes  );
    QCOMPARE(obj["record_UTF"      ].toString(),record_UTF      );

    dir.removeRecursively();
}

void QJsonCompatibility::test_1_latin()
{
    //QSKIP("ALREADY COMPLETE");

    QString dirpath = scopeDirPath+"/test_1_latin";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";


    std::string name1 = "Str1";
    std::string val1  = "Val1";
    std::string name2 = "Jkds dkfd";
    std::string val2  = "    ";
    std::string name3 = "Josef";
    std::string val3  = "Nope__x8";
    jjson17::Object w_obj;
                    w_obj.insert({name1,val1});
                    w_obj.insert({name2,val2});
                    w_obj.insert({name3,val3});
    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_obj;
                  ofs.close();

    READ_FILE_BACK();

    auto r_obj = doc.object();
    QVERIFY(!r_obj.isEmpty());
    QCOMPARE(r_obj[QString::fromStdString(name1)].toString().toStdString(), val1);
    QCOMPARE(r_obj[QString::fromStdString(name2)].toString().toStdString(), val2);
    QCOMPARE(r_obj[QString::fromStdString(name3)].toString().toStdString(), val3);

    dir.removeRecursively();
}

void QJsonCompatibility::test_2_reals()
{
    //QSKIP("ALREADY COMPLETE");

    QString dirpath = scopeDirPath+"/test_2_reals";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";

    double      val1  = 28942.42;
    double      val2  = 37e10   ;
    double      val3  = 1./3    ;
    double      val4  = 1.7/1e10;
    float       val5  = 1.25f;
    float       val6  = -0.5625f;
    float       val7  = 0.375f;
    jjson17::Array  w_arr{ val1,
                           val2,
                           val3,
                           val4,
                           val5,
                           val6,
                           val7
                         };

    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_arr;
                  ofs.close();

    READ_FILE_BACK();

    auto r_arr = doc.array();
    QVERIFY(!r_arr.isEmpty());
    QCOMPARE(r_arr[0].toDouble(), val1);
    QCOMPARE(r_arr[1].toDouble(), val2);
    QCOMPARE(r_arr[2].toDouble(), val3);
    QCOMPARE(r_arr[3].toDouble(), val4);
    QCOMPARE(r_arr[4].toDouble(), val5);
    QCOMPARE(r_arr[5].toDouble(), val6);
    QCOMPARE(r_arr[6].toDouble(), val7);
    dir.removeRecursively();
}

void QJsonCompatibility::test_3_integers()
{
    //QSKIP("ALREADY COMPLETE");
    QString dirpath = scopeDirPath+"/test_3_integers";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";

    int            val0  = -28942;
    short          val1  = 37e2;
    qint64         val2  = 12345678ll<<10;
    unsigned short val3  = -4000;
    char           val4  = 27;
    unsigned char  val5  = 27;
    char           val6  = -70;
    uint8_t        val7  = 33;
    int8_t         val8  = -33;

    jjson17::Value v{3ull};

    jjson17::Array  w_arr{ val0, val1, val2,
                           val3, val4, val5,
                           val6, val7, val8,
                         };

    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_arr;
                  ofs.close();

    READ_FILE_BACK();

    auto r_arr = doc.array();
    QVERIFY(!r_arr.isEmpty());
    QCOMPARE(r_arr[0].toInt(), val0);
    QCOMPARE(r_arr[1].toInt(), val1);
    QCOMPARE(r_arr[2].toDouble(), val2);    //большое значение для int. Ограничение QJson
    QCOMPARE(r_arr[3].toInt(), val3);
    QCOMPARE(r_arr[4].toInt(), val4);
    QCOMPARE(r_arr[5].toInt(), val5);
    QCOMPARE(r_arr[6].toInt(), val6);
    QCOMPARE(r_arr[7].toInt(), val7);
    QCOMPARE(r_arr[8].toInt(), val8);

    dir.removeRecursively();
}

struct Test_Staff {
    QString name;
    QString position;
    QVector<QString> subordinates;
    double salary;
    unsigned char age;
    bool newbi;
};

void QJsonCompatibility::test_4_mix_latin_nums()
{
    using namespace jjson17;
    //QSKIP("ALREADY COMPLETE");
    try {
    QString dirpath = scopeDirPath+"/test_4_mix_latin_nums";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";

    auto convert = [](const Test_Staff& t) -> Object
    {
        Object r {
        {"name"    ,t.name.toStdString()      },
        {"position",t.position.toStdString()  },
        {"salary"  ,t.salary    },
        {"age"     ,t.age       },
        {"newbi"   ,t.newbi     }
        };
        if(t.subordinates.isEmpty())
            r.insert({"subordinates",nullptr});
        else {
            Array lst;
            foreach(auto s , t.subordinates)
                lst.push_back(s.toStdString());
            r.insert({"subordinates",lst});
        }
        return r;
    };

    Test_Staff t1 {
        "Katrin",
        "sniper",
        {"Fich","Bik"},
        1250.7,
        3,
        false
    };
    Test_Staff t2 {
        "Fich",
        "officer",
        {},
        1500.23,
        38,
        false
    };
    Test_Staff t3 {
        "Bik",
        "mascot",
        {},
        0,
        50,
        true
    };

    Object w_squad;
    Array  w_arr {33.3,10.2,111,4000,"bravo",nullptr};

    w_squad.insert({t1.name.toStdString(),convert(t1)});
    w_squad.insert({t2.name.toStdString(),convert(t2)});
    w_squad.insert({t3.name.toStdString(),convert(t3)});
    w_squad.insert({"something",w_arr});

    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_squad;
                  ofs.close();

    READ_FILE_BACK();

    auto r_obj = doc.object();
    auto r_arr = r_obj["something"].toArray();
        QCOMPARE(std::get<double >(w_arr[0]),r_arr[0].toDouble());
        QCOMPARE(std::get<double >(w_arr[1]),r_arr[1].toDouble());
        QCOMPARE(std::get<int64_t>(w_arr[2]),r_arr[2].toInt   ());
        QCOMPARE(std::get<int64_t>(w_arr[3]),r_arr[3].toInt   ());
        QCOMPARE(std::get<std::string>(w_arr[4]),r_arr[4].toString().toStdString());
        QVERIFY(r_arr[5].isNull());

    auto r_obj_lvl2 = r_obj[t1.name].toObject();
        QCOMPARE(t1.name    ,r_obj_lvl2["name"].toString());
        QCOMPARE(t1.position,r_obj_lvl2["position"].toString());
        QCOMPARE(t1.salary  ,r_obj_lvl2["salary"].toDouble());
        QCOMPARE(t1.age     ,r_obj_lvl2["age"].toInt());
        QCOMPARE(t1.newbi   ,r_obj_lvl2["newbi"].toBool());
        QVERIFY(!r_obj_lvl2["subordinates"].isNull());
        auto r_obj_lvl2_arr = r_obj_lvl2["subordinates"].toArray();
             QVERIFY(r_obj_lvl2_arr.size()==2);
             QCOMPARE(r_obj_lvl2_arr[0],t1.subordinates[0]);
             QCOMPARE(r_obj_lvl2_arr[1],t1.subordinates[1]);

    r_obj_lvl2 = r_obj[t2.name].toObject();
        QCOMPARE(t2.name    ,r_obj_lvl2["name"].toString());
        QCOMPARE(t2.position,r_obj_lvl2["position"].toString());
        QCOMPARE(t2.salary  ,r_obj_lvl2["salary"].toDouble());
        QCOMPARE(t2.age     ,r_obj_lvl2["age"].toInt());
        QCOMPARE(t2.newbi   ,r_obj_lvl2["newbi"].toBool());
        QVERIFY(r_obj_lvl2["subordinates"].isNull());

    r_obj_lvl2 = r_obj[t3.name].toObject();
        QCOMPARE(t3.name    ,r_obj_lvl2["name"].toString());
        QCOMPARE(t3.position,r_obj_lvl2["position"].toString());
        QCOMPARE(t3.salary  ,r_obj_lvl2["salary"].toDouble());
        QCOMPARE(t3.age     ,r_obj_lvl2["age"].toInt());
        QCOMPARE(t3.newbi   ,r_obj_lvl2["newbi"].toBool());
        QVERIFY(r_obj_lvl2["subordinates"].isNull());

    dir.removeRecursively();
    } catch(const std::exception& e) {
        QFAIL(("EXCEPTION: "+QString(e.what())).toLatin1());
    } catch(...) {
        QFAIL("UNSTD EXCEPTION");
    }
}

void QJsonCompatibility::test_5_unicode_1()
{
    using namespace jjson17;
    //QSKIP("ALREADY COMPLETE");
    QString dirpath = scopeDirPath+"/test_5_unicode_1";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";

    Test_Staff s {
        "Русский текст",
        "正在發展這個協",
        {"點對點（Wi-Fi Peer-to-Peer）"," نوعية واحدة م","domésticas más","기술적 설명"},
        -350,
        10,
        true
    };


    Object w_obj{{"name"    ,s.name.toStdString()},
                 {"position",s.position.toStdString()},
                 {"salary"  ,s.salary},
                 {"age"     ,s.age},
                 {"newbi"   ,s.newbi},
                 {"subordinates",Array()}
                };
    foreach(auto m , s.subordinates)
        w_obj["subordinates"].push_back(m.toStdString());

    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_obj;
                  ofs.close();

    READ_FILE_BACK();

    auto r_obj = doc.object();
    QCOMPARE(s.name     , r_obj["name"      ].toString());
    QCOMPARE(s.position , r_obj["position"  ].toString());
    QCOMPARE(s.salary   , r_obj["salary"    ].toInt   ());
    QCOMPARE(s.age      , r_obj["age"       ].toInt   ());
    QCOMPARE(s.newbi    , r_obj["newbi"     ].toBool  ());
    auto r_arr = r_obj["subordinates"].toArray();
        QCOMPARE(s.subordinates[0],r_arr[0].toString());
        QCOMPARE(s.subordinates[1],r_arr[1].toString());
        QCOMPARE(s.subordinates[2],r_arr[2].toString());
        QCOMPARE(s.subordinates[3],r_arr[3].toString());

    dir.removeRecursively();
}

void QJsonCompatibility::test_6_escapeseq_1()
{
    using namespace jjson17;
    //QSKIP("ALREADY COMPLETE");
    QString dirpath = scopeDirPath+"/test_6_escapeseq_1";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";

    QStringList sl {
        u8"AAA\"BBB"  ,
        u8"AAA\'BBB"  ,
        u8"AAA\\BBB"  ,
        u8"AAA/BBB"   ,
        u8"AAA\bBBB"  ,
        u8"AAA\fBBB"  ,
        u8"AAA\nBBB"  ,
        u8"AAA\rBBB"  ,
        u8"AAA\tBBB"  ,
        u8"AAA\u2211BBB"
    };

    Array w_arr;
          w_arr.reserve(sl.size());
    foreach (auto s, sl) {
        w_arr.push_back(s.toStdString());
    }

    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_arr;
                  ofs.close();

    READ_FILE_BACK();

    auto r_arr = doc.array();
    QCOMPARE(r_arr.size(),sl.size());
    auto i = 0;
    foreach (auto var, r_arr)
    {
        QCOMPARE(var.toString(),sl[i]);
        i++;
    }

    dir.removeRecursively();
}

void QJsonCompatibility::test_7_incaps_structures_1()
{
    using namespace jjson17;
    //QSKIP("ALREADY COMPLETE");
    QString dirpath = scopeDirPath+"/test_7_incaps_structures_1";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QString filepath = dirpath+"/test.json";

    Object w_obj_lvl3  {{"name","Alex"},{"Один",123},{"Два",77},{u8"\u2211",200}};
    Object w_obj_lvl2a {{"Level3",w_obj_lvl3},{"Jin",nullptr}};
    Object w_obj_lvl2b {{"Cat",Array{33,37.8,nullptr,"fur"}},{"Flag",true}};
    Object w_obj_lvl1  {{"Level2A",w_obj_lvl2a},{"Level2B",w_obj_lvl2b}};

    std::ofstream ofs;
                  ofs.precision(12);
                  ofs.open(filepath.toStdString());
                  ofs << w_obj_lvl1;
                  ofs.close();

    READ_FILE_BACK();

    auto r_obj = doc.object();
    auto r_obj_lvl2a = r_obj["Level2A"].toObject();
    auto r_obj_lvl2b = r_obj["Level2B"].toObject();
    QVERIFY(!r_obj_lvl2a.isEmpty());
    QVERIFY(!r_obj_lvl2b.isEmpty());

    auto r_obj_lvl3 = r_obj_lvl2a["Level3"].toObject();
    QVERIFY(!r_obj_lvl3.isEmpty());
    QCOMPARE(r_obj_lvl3[u8"\u2211"].toInt(),200);

    dir.removeRecursively();
}

struct Struct {
    std::string somestr;
    short       someval;
    Struct*     other{nullptr};
};
jjson17::Object asJsonObject(const Struct& s)
{
    namespace json = jjson17;
    json::Object obj{
                {"somestr",s.somestr},
                {"someval",s.someval},
                };
    if(s.other)
         obj.insert({"other",asJsonObject(*s.other)});
    else obj.insert({"other",nullptr});
    return obj;
};
void QJsonCompatibility::test_8_to_string()
{
    namespace json = jjson17;

    //QSKIP("ALREADY COMPLETE");
    Struct s1 {"SSS",-10};
    Struct s2 {"GGG",-100,&s1};
    json::Record r = {"TheRecord",asJsonObject(s2)};
    std::string etalon = "\"TheRecord\":\t\n"
                         "{\n"
                            "\t\"other\":\t\n"
                            "\t{\n"
                            "\t\t\"other\":\tnull,\n"
                            "\t\t\"somestr\":\t\"SSS\",\n"
                            "\t\t\"someval\":\t-10\n"
                         "\t},\n"
                         "\t\"somestr\":\t\"GGG\",\n"
                         "\t\"someval\":\t-100\n"
                         "}";
    auto result = json::to_string(r);
    QCOMPARE(result,etalon);
}

void QJsonCompatibility::test_9_implicit_cast()
{
    using namespace jjson17;
    using std::string, std::get;
    //QSKIP("ALREADY COMPLETE");

    Array  testArr = {
        "ThisIsString",3.14,9.8,0.28f,nullptr,8,true,Array{11.,12.,14.},Object{{"Y-Y",70},{"X-X","Xerx"}}
    };

    string   str = testArr[0];
    double   pi  = testArr[1];
    unsigned g   = testArr[2];                  //Неавное приведение выполняет также неявное преобразование из floating-point в integer мат-округлением
    double   d   = testArr[3];
    auto     nul = get<nullptr_t>(testArr[4]);
    int      i   = testArr[5];
    double   i2d = testArr[5];                  //Неавное приведение выполняет также неявное преобразование из integer в floating-point
    bool     b   = testArr[6];
    Array    arr = testArr[7];
    Object   obj = testArr[8];

    QCOMPARE(str,get<string>(testArr[0]));
    QCOMPARE(pi ,get<double>(testArr[1]));
    QCOMPARE(g  ,std::round(get<double>(testArr[2])));
    QCOMPARE(d  ,get<double >(testArr[3]));
    QCOMPARE(nul,nullptr);
    QCOMPARE(i  ,get<int64_t>(testArr[5]));
    QCOMPARE(i2d,double(get<int64_t>(testArr[5])));
    QCOMPARE(b  ,get<bool   >(testArr[6]));
    QCOMPARE(arr,get<Array  >(testArr[7]));
    QCOMPARE(obj,get<Object >(testArr[8]));

    bool goodException{false};
    try{int g = get<int64_t>(testArr[2]); Q_UNUSED(g); } catch(const std::bad_variant_access& e) {goodException=true;}
    QVERIFY(goodException);
}


template <typename PathType>
static PathType getBinDir() {
#ifdef Q_OS_WINDOWS
    return "C:\\Windows\\";
#elif Q_OS_UNIX
    return "/usr/bin/";
#else
    static_assert(false,"Unsupported OS")
#endif
}

//recursive dirscan
static jjson17::Object subScanFunc (std::filesystem::directory_entry entry, int depth, const int MAX_DEPTH=2, const int MAX_ELEMS_AT_LVL=-1)
{
    namespace json = jjson17;
    namespace fs = std::filesystem;

    json::Object obj {
        {"Directory",entry.is_directory()},
        {"Size",entry.file_size()},
        {"Permitions",static_cast<int>(entry.status().permissions())},
        {"Depth",depth}
    };

    if(entry.is_directory() && depth < MAX_DEPTH)
    {
        depth++;
        json::Object content;
        int cnt = MAX_ELEMS_AT_LVL;
        for(const auto& d : fs::directory_iterator(entry,fs::directory_options::skip_permission_denied)) {
            content.insert( {d.path().filename().generic_string() , subScanFunc(d,depth,MAX_DEPTH,MAX_ELEMS_AT_LVL)} );//content.insert( {d.path().filename().u8string() , subScanFunc(d,depth)} );
            if(cnt--==0) break;
        }
        obj.insert({"content",content});
    }
    return obj;
}
//recursive dirscan
static QJsonObject subScanFunc (const QFileInfo& entry, int depth, const int MAX_DEPTH=2, const int MAX_ELEMS_AT_LVL=-1)
{
    QJsonObject obj {
        {"Directory",entry.isDir()},
        {"Size",entry.size()},
        {"Permitions",static_cast<int>(entry.permissions())},
        {"Depth",depth}
    };

    if(entry.isDir() && depth < MAX_DEPTH)
    {
        depth++;
        QJsonObject content;
        int cnt = MAX_ELEMS_AT_LVL;
        foreach(const auto& d , QDir(entry.filePath()).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries)) {
            content.insert(d.fileName(),subScanFunc(d,depth,MAX_DEPTH,MAX_ELEMS_AT_LVL));
            if(cnt--==0) break;
        }
        obj.insert("content",content);
    }
    return obj;
}

#define RAW_OFSTREAM    // служит для изменения способа записи напрямую через ofstream или через sstream
void QJsonCompatibility::perf_1_dirscan()
{
    namespace json = jjson17;
    namespace fs = std::filesystem;
    using namespace std::chrono;

    //QSKIP("ALREADY COMPLETE");

    QString dirpath = scopeDirPath+"/perf_1_dirscan";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);

    //....... std-way  .......
    uint32_t jjTotal{0};
    {
        fs::directory_entry stdDir("C:/Windows/");//fs::directory_entry stdDir(getBinDir<fs::path>());
        QVERIFY(stdDir.exists());
        auto before = steady_clock::now();
        json::Object jsonDirRoot = subScanFunc(stdDir,0);
        auto after = steady_clock::now();
        auto ms = duration_cast<std::chrono::milliseconds>(after - before);
        qDebug() << "JJSON17. time to build :"<<ms.count()<<"ms";
        jjTotal+=ms.count();
        QString filepath = dirpath+"/test_jj17.json";
        before = steady_clock::now();
      #ifdef RAW_OFSTREAM
            std::ofstream ofs;
                          ofs.precision(12);
                          ofs.open(filepath.toStdString());
                          ofs << jsonDirRoot;
                          ofs.close();
      #else

            std::stringstream ss;
                              ss.precision(12);
                              ss << jsonDirRoot;
            std::ofstream ofs;
                          ofs.open(filepath.toStdString());
                          ofs << ss.str();
                          ofs.close();
      #endif
        after = steady_clock::now();
        ms = duration_cast<std::chrono::milliseconds>(after - before);
        jjTotal+=ms.count();
        qDebug() << "JJSON17. time to write :"<<ms.count()<<"ms";
        qDebug() << "JJSON17. total :"<<jjTotal<<"ms";
    }
    //........................
    //....... QT-way  ........

    uint32_t qTotal{0};
    {
        QFileInfo qDir(getBinDir<QString>());
        QVERIFY(qDir.exists());
        auto before = steady_clock::now();
        auto jsonDirRoot = subScanFunc(qDir,0);
        auto after = steady_clock::now();
        auto ms = duration_cast<std::chrono::milliseconds>(after - before);
        qDebug() << "QJSON. time to build :"<<ms.count()<<"ms";
        qTotal+=ms.count();

        QString filepath = dirpath+"/test_q.json";
        before = steady_clock::now();
            QJsonDocument qjsdoc(jsonDirRoot);
            QFile   f(filepath);
                    f.open(QIODevice::WriteOnly);
                    f.write(qjsdoc.toJson());
                    f.close();
        after = steady_clock::now();
        ms = duration_cast<std::chrono::milliseconds>(after - before);
        qTotal+=ms.count();
        qDebug() << "QJSON. time to write :"<<ms.count()<<"ms";
        qDebug() << "QJSON. total :"<<qTotal<<"ms";
    }

    //........................
    qDebug()  << "JJSON vs QJSON"<< double(jjTotal)/qTotal<<"the less the best";

    dir.removeRecursively();
}

void QJsonCompatibility::perf_2_dirscan()
{
    namespace json = jjson17;
    namespace fs = std::filesystem;
    using namespace std::chrono;

    //QSKIP("ALREADY COMPLETE");

    const int MAX_DEPTH = 20;
    const int MAX_ELEMS_AT_LVL = 20;

    QString dirpath = scopeDirPath+"/perf_2_dirscan";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);

    //....... std-way  .......
    uint32_t jjTotal{0};
    {
        fs::directory_entry stdDir("C:/");
        QVERIFY(stdDir.exists());
        auto before = steady_clock::now();
        json::Object jsonDirRoot = subScanFunc(stdDir,0,MAX_DEPTH,MAX_ELEMS_AT_LVL);
        auto after = steady_clock::now();
        auto ms = duration_cast<std::chrono::milliseconds>(after - before);
        qDebug() << "JJSON17. time to build :"<<ms.count()<<"ms";
        jjTotal+=ms.count();
        QString filepath = dirpath+"/test_jj17.json";
        before = steady_clock::now();
      #ifdef RAW_OFSTREAM
            std::ofstream ofs;
                          ofs.precision(12);
                          ofs.open(filepath.toStdString());
                          ofs << jsonDirRoot;
                          ofs.close();
      #else

            std::stringstream ss;
                              ss.precision(12);
                              ss << jsonDirRoot;
            std::ofstream ofs;
                          ofs.open(filepath.toStdString());
                          ofs << ss.str();
                          ofs.close();
      #endif
        after = steady_clock::now();
        ms = duration_cast<std::chrono::milliseconds>(after - before);
        jjTotal+=ms.count();
        qDebug() << "JJSON17. time to write :"<<ms.count()<<"ms";
        qDebug() << "JJSON17. total :"<<jjTotal<<"ms";
    }
    //........................
    //....... QT-way  ........

    uint32_t qTotal{0};
    {
        QFileInfo qDir("C:/");
        QVERIFY(qDir.exists());
        auto before = steady_clock::now();
        auto jsonDirRoot = subScanFunc(qDir,0,MAX_DEPTH,MAX_ELEMS_AT_LVL);
        auto after = steady_clock::now();
        auto ms = duration_cast<std::chrono::milliseconds>(after - before);
        qDebug() << "QJSON. time to build :"<<ms.count()<<"ms";
        qTotal+=ms.count();

        QString filepath = dirpath+"/test_q.json";
        before = steady_clock::now();
            QJsonDocument qjsdoc(jsonDirRoot);
            QFile   f(filepath);
                    f.open(QIODevice::WriteOnly);
                    f.write(qjsdoc.toJson());
                    f.close();
        after = steady_clock::now();
        ms = duration_cast<std::chrono::milliseconds>(after - before);
        qTotal+=ms.count();
        qDebug() << "QJSON. time to write :"<<ms.count()<<"ms";
        qDebug() << "QJSON. total :"<<qTotal<<"ms";
    }

    //........................
    qDebug()  << "JJSON vs QJSON"<< double(jjTotal)/qTotal<<"the less the best";

    dir.removeRecursively();
}

void QJsonCompatibility::parse_1_numbers()
{
    using namespace std;
    using namespace jjson17;

    //QSKIP("ALREADY COMPLETE");

    QString dirpath  = scopeDirPath+"/parse_1_numbers";
    QString filepath = dirpath+"/test_q.json";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QJsonObject qjsobj;
                qjsobj.insert("First"  ,1);
                qjsobj.insert("Second" ,2);
                qjsobj.insert("Third"  ,1.11);
                qjsobj.insert("Forth"  ,-9);
                qjsobj.insert("Fifth"  ,-0.625);
                qjsobj.insert("Sixth"  ,0);
                qjsobj.insert("Seventh",-2.125);
    QJsonDocument qjsdoc(qjsobj);
    QFile   f(filepath);
            f.open(QIODevice::WriteOnly);
            f.write(qjsdoc.toJson());
            f.close();
    ifstream infile;
             infile.open(filepath.toStdString());
    auto jj17doc = parse(infile);
             infile.close();
    QVERIFY(holds_alternative<Object>(jj17doc));
    Object& jj17obj = get<Object>(jj17doc);
    QCOMPARE(jj17obj.size(),7);
    for(const auto& v : jj17obj)
        qDebug() << QString::fromStdString(v.first) << v.second.index();
    QVERIFY(holds_alternative<int64_t>(jj17obj["First"  ]));
    QVERIFY(holds_alternative<int64_t>(jj17obj["Second" ]));
    QVERIFY(holds_alternative<double >(jj17obj["Third"  ]));
    QVERIFY(holds_alternative<int64_t>(jj17obj["Forth"  ]));
    QVERIFY(holds_alternative<double >(jj17obj["Fifth"  ]));
    QVERIFY(holds_alternative<int64_t>(jj17obj["Sixth"  ]));
    QVERIFY(holds_alternative<double >(jj17obj["Seventh"]));
    QCOMPARE(get<int64_t>(jj17obj["First"  ]),1);
    QCOMPARE(get<int64_t>(jj17obj["Second" ]),2);
    QCOMPARE(get<double >(jj17obj["Third"  ]),1.11);
    QCOMPARE(get<int64_t>(jj17obj["Forth"  ]),-9);
    QCOMPARE(get<double >(jj17obj["Fifth"  ]),-0.625);
    QCOMPARE(get<int64_t>(jj17obj["Sixth"  ]),0);
    QCOMPARE(get<double >(jj17obj["Seventh"]),-2.125);

    QJsonArray qjsarr = { 3,4,2.55,-390,-0.078,0,-10.001 };
    qjsdoc.setArray(qjsarr);
    filepath = dirpath+"/test_q2.json";
        f.setFileName(filepath);
        f.open(QIODevice::WriteOnly);
        f.write(qjsdoc.toJson());
        f.close();
    infile.open(filepath.toStdString());
    jj17doc = parse(infile);
    infile.close();
    QVERIFY(holds_alternative<Array>(jj17doc));
    Array& jj17arr = get<Array>(jj17doc);
    QCOMPARE(jj17arr.size(),7);
    QVERIFY(holds_alternative<int64_t>(jj17arr[0]));
    QVERIFY(holds_alternative<int64_t>(jj17arr[1]));
    QVERIFY(holds_alternative<double >(jj17arr[2]));
    QVERIFY(holds_alternative<int64_t>(jj17arr[3]));
    QVERIFY(holds_alternative<double >(jj17arr[4]));
    QVERIFY(holds_alternative<int64_t>(jj17arr[5]));
    QVERIFY(holds_alternative<double >(jj17arr[6]));
    QCOMPARE(get<int64_t>(jj17arr[0]),3);
    QCOMPARE(get<int64_t>(jj17arr[1]),4);
    QCOMPARE(get<double >(jj17arr[2]),2.55);
    QCOMPARE(get<int64_t>(jj17arr[3]),-390);
    QCOMPARE(get<double >(jj17arr[4]),-0.078);
    QCOMPARE(get<int64_t>(jj17arr[5]),0);
    QCOMPARE(get<double >(jj17arr[6]),-10.001);


    dir.removeRecursively();
}

void QJsonCompatibility::parse_2_numbers_self()
{
    using namespace std;
    using namespace jjson17;
    //QSKIP("ALREADY COMPLETE");

    QString dirpath  = scopeDirPath+"/parse_2_numbers_self";
    std::string filepath = dirpath.toStdString()+"/test_q.json";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    Object jj17obj;
           jj17obj.insert({"First"  ,1      });
           jj17obj.insert({"Second" ,2      });
           jj17obj.insert({"Third"  ,1.11   });
           jj17obj.insert({"Forth"  ,-9     });
           jj17obj.insert({"Fifth"  ,-0.625 });
           jj17obj.insert({"Sixth"  ,0      });
           jj17obj.insert({"Seventh",-2.125 });
    ofstream outfile;
             outfile.open(filepath);
             outfile << jj17obj;
             outfile.close();
    ifstream infile;
             infile.open(filepath);
    auto jj17doc = parse(infile);
             infile.close();
    QVERIFY(holds_alternative<Object>(jj17doc));
    Object& jj17obj_back = get<Object>(jj17doc);
    QCOMPARE(jj17obj_back.size(),7);
    QVERIFY(holds_alternative<int64_t>(jj17obj_back["First"  ]));
    QVERIFY(holds_alternative<int64_t>(jj17obj_back["Second" ]));
    QVERIFY(holds_alternative<double >(jj17obj_back["Third"  ]));
    QVERIFY(holds_alternative<int64_t>(jj17obj_back["Forth"  ]));
    QVERIFY(holds_alternative<double >(jj17obj_back["Fifth"  ]));
    QVERIFY(holds_alternative<int64_t>(jj17obj_back["Sixth"  ]));
    QVERIFY(holds_alternative<double >(jj17obj_back["Seventh"]));
    QCOMPARE(get<int64_t>(jj17obj_back["First"  ]),1);
    QCOMPARE(get<int64_t>(jj17obj_back["Second" ]),2);
    QCOMPARE(get<double >(jj17obj_back["Third"  ]),1.11);
    QCOMPARE(get<int64_t>(jj17obj_back["Forth"  ]),-9);
    QCOMPARE(get<double >(jj17obj_back["Fifth"  ]),-0.625);
    QCOMPARE(get<int64_t>(jj17obj_back["Sixth"  ]),0);
    QCOMPARE(get<double >(jj17obj_back["Seventh"]),-2.125);

    Array jj17arr = { 3,4,2.55,-390,-0.078,0,-10.001 };
    filepath = dirpath.toStdString()+"/test_q2.json";
        outfile.open(filepath);
        outfile << jj17arr;
        outfile.close();
    infile.open(filepath);
    jj17doc = parse(infile);
    infile.close();

    QVERIFY(holds_alternative<Array>(jj17doc));
    const Array& jj17arr_back = get<Array>(jj17doc);
    QCOMPARE(jj17arr_back.size(),7);
    QVERIFY(holds_alternative<int64_t>(jj17arr_back[0]));
    QVERIFY(holds_alternative<int64_t>(jj17arr_back[1]));
    QVERIFY(holds_alternative<double >(jj17arr_back[2]));
    QVERIFY(holds_alternative<int64_t>(jj17arr_back[3]));
    QVERIFY(holds_alternative<double >(jj17arr_back[4]));
    QVERIFY(holds_alternative<int64_t>(jj17arr_back[5]));
    QVERIFY(holds_alternative<double >(jj17arr_back[6]));
    QCOMPARE(get<int64_t>(jj17arr_back[0]),3);
    QCOMPARE(get<int64_t>(jj17arr_back[1]),4);
    QCOMPARE(get<double >(jj17arr_back[2]),2.55);
    QCOMPARE(get<int64_t>(jj17arr_back[3]),-390);
    QCOMPARE(get<double >(jj17arr_back[4]),-0.078);
    QCOMPARE(get<int64_t>(jj17arr_back[5]),0);
    QCOMPARE(get<double >(jj17arr_back[6]),-10.001);


    dir.removeRecursively();
}
void QJsonCompatibility::parse_3_latin()
{
    using namespace std;
    using namespace jjson17;

    //QSKIP("ALREADY COMPLETE");
    QStringList testList = {"1","null","abracadabra","Jerom K."," Helicopter \"K52\"","moonlight\nsunlight","good morning, little mouse!","object{N}",""};

    QString dirpath  = scopeDirPath+"/parse_3_latin";
    QString filepath = dirpath+"/test_q.json";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QJsonObject qjsobj;
                qjsobj.insert("First 1"  , testList[0]);
                qjsobj.insert("Second 2" , testList[1]);
                qjsobj.insert("Third 3"  , testList[2]);
                qjsobj.insert("Forth 4"  , testList[3]);
                qjsobj.insert("Fifth 5"  , testList[4]);
                qjsobj.insert("Sixth 6"  , testList[5]);
                qjsobj.insert("Seventh 7", testList[6]);
                qjsobj.insert("Eighth 8" , testList[7]);
                qjsobj.insert("Ninth 9"  , testList[8]);
    QJsonDocument qjsdoc(qjsobj);
    QFile   f(filepath);
            f.open(QIODevice::WriteOnly);
            f.write(qjsdoc.toJson());
            f.close();
    ifstream infile;
             infile.open(filepath.toStdString());
    auto jj17doc = parse(infile);
             infile.close();
    QVERIFY(holds_alternative<Object>(jj17doc));
    Object& jj17obj = get<Object>(jj17doc);
    QCOMPARE(jj17obj.size(),testList.size());
    for(const auto &[k,v] : jj17obj)
        qDebug() << QString::fromStdString(to_string(Record(k,v)));
    QVERIFY(holds_alternative<string>(jj17obj["First 1"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Second 2" ]));
    QVERIFY(holds_alternative<string>(jj17obj["Third 3"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Forth 4"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Fifth 5"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Sixth 6"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Seventh 7"]));
    QVERIFY(holds_alternative<string>(jj17obj["Eighth 8" ]));
    QVERIFY(holds_alternative<string>(jj17obj["Ninth 9"  ]));
    for(const auto &[k,v] : jj17obj)
        qDebug() << QString::fromStdString(get<string>(v));
    QCOMPARE(get<string>(jj17obj["First 1"  ]), testList[0].toStdString());
    QCOMPARE(get<string>(jj17obj["Second 2" ]), testList[1].toStdString());
    QCOMPARE(get<string>(jj17obj["Third 3"  ]), testList[2].toStdString());
    QCOMPARE(get<string>(jj17obj["Forth 4"  ]), testList[3].toStdString());
    QCOMPARE(get<string>(jj17obj["Fifth 5"  ]), testList[4].toStdString());
    QCOMPARE(get<string>(jj17obj["Sixth 6"  ]), testList[5].toStdString());
    QCOMPARE(get<string>(jj17obj["Seventh 7"]), testList[6].toStdString());
    QCOMPARE(get<string>(jj17obj["Eighth 8" ]), testList[7].toStdString());
    QCOMPARE(get<string>(jj17obj["Ninth 9"  ]), testList[8].toStdString());

    QJsonArray qjsarr;
    foreach (const auto& s, testList)
        qjsarr.append(s);
    qjsdoc.setArray(qjsarr);
    filepath = dirpath+"/test_q2.json";
        f.setFileName(filepath);
        f.open(QIODevice::WriteOnly);
        f.write(qjsdoc.toJson());
        f.close();
    infile.open(filepath.toStdString());
    jj17doc = parse(infile);
    infile.close();
    QVERIFY(holds_alternative<Array>(jj17doc));
    Array& jj17arr = get<Array>(jj17doc);
    QCOMPARE(jj17arr.size(),testList.size());
    QVERIFY(holds_alternative<string>(jj17arr[0]));
    QVERIFY(holds_alternative<string>(jj17arr[1]));
    QVERIFY(holds_alternative<string>(jj17arr[2]));
    QVERIFY(holds_alternative<string>(jj17arr[3]));
    QVERIFY(holds_alternative<string>(jj17arr[4]));
    QVERIFY(holds_alternative<string>(jj17arr[5]));
    QVERIFY(holds_alternative<string>(jj17arr[6]));
    QVERIFY(holds_alternative<string>(jj17arr[7]));
    QVERIFY(holds_alternative<string>(jj17arr[8]));
    QCOMPARE(get<string>(jj17arr[0]), testList[0].toStdString());
    QCOMPARE(get<string>(jj17arr[1]), testList[1].toStdString());
    QCOMPARE(get<string>(jj17arr[2]), testList[2].toStdString());
    QCOMPARE(get<string>(jj17arr[3]), testList[3].toStdString());
    QCOMPARE(get<string>(jj17arr[4]), testList[4].toStdString());
    QCOMPARE(get<string>(jj17arr[5]), testList[5].toStdString());
    QCOMPARE(get<string>(jj17arr[6]), testList[6].toStdString());
    QCOMPARE(get<string>(jj17arr[7]), testList[7].toStdString());
    QCOMPARE(get<string>(jj17arr[8]), testList[8].toStdString());

    dir.removeRecursively();
}

void QJsonCompatibility::parse_3_latin_self()
{
    using namespace std;
    using namespace jjson17;

    //QSKIP("ALREADY COMPLETE");
    vector<string> testList = {"1","null","abracadabra","Jerom K."," Helicopter \"K52\"","moonlight\nsunlight","good morning, little mouse!","object{N}",""};

    QString dirpath = scopeDirPath+"/parse_3_latin_self";
    string filepath = dirpath.toStdString()+"/test_q.json";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    Object jj17obj_w;
                jj17obj_w.insert({"First 1"  , testList[0]});
                jj17obj_w.insert({"Second 2" , testList[1]});
                jj17obj_w.insert({"Third 3"  , testList[2]});
                jj17obj_w.insert({"Forth 4"  , testList[3]});
                jj17obj_w.insert({"Fifth 5"  , testList[4]});
                jj17obj_w.insert({"Sixth 6"  , testList[5]});
                jj17obj_w.insert({"Seventh 7", testList[6]});
                jj17obj_w.insert({"Eighth 8" , testList[7]});
                jj17obj_w.insert({"Ninth 9"  , testList[8]});
    ofstream outfile;
             outfile.open(filepath);
             outfile << jj17obj_w;
             outfile.close();
    ifstream infile;
             infile.open(filepath);
    auto jj17doc = parse(infile);
             infile.close();
    QVERIFY(holds_alternative<Object>(jj17doc));
    Object& jj17obj = get<Object>(jj17doc);
    QCOMPARE(jj17obj.size(),testList.size());
    for(const auto &[k,v] : jj17obj)
        qDebug() << QString::fromStdString(to_string(Record(k,v)));
    QVERIFY(holds_alternative<string>(jj17obj["First 1"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Second 2" ]));
    QVERIFY(holds_alternative<string>(jj17obj["Third 3"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Forth 4"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Fifth 5"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Sixth 6"  ]));
    QVERIFY(holds_alternative<string>(jj17obj["Seventh 7"]));
    QVERIFY(holds_alternative<string>(jj17obj["Eighth 8" ]));
    QVERIFY(holds_alternative<string>(jj17obj["Ninth 9"  ]));
    for(const auto &[k,v] : jj17obj)
        qDebug() << QString::fromStdString(get<string>(v));
    QCOMPARE(get<string>(jj17obj["First 1"  ]), testList[0]);
    QCOMPARE(get<string>(jj17obj["Second 2" ]), testList[1]);
    QCOMPARE(get<string>(jj17obj["Third 3"  ]), testList[2]);
    QCOMPARE(get<string>(jj17obj["Forth 4"  ]), testList[3]);
    QCOMPARE(get<string>(jj17obj["Fifth 5"  ]), testList[4]);
    QCOMPARE(get<string>(jj17obj["Sixth 6"  ]), testList[5]);
    QCOMPARE(get<string>(jj17obj["Seventh 7"]), testList[6]);
    QCOMPARE(get<string>(jj17obj["Eighth 8" ]), testList[7]);
    QCOMPARE(get<string>(jj17obj["Ninth 9"  ]), testList[8]);

    Array jj17arr_w;
    foreach (const auto& s, testList)
        jj17arr_w.push_back(s);
    filepath = dirpath.toStdString()+"/test_q2.json";
    outfile.open(filepath);
    outfile << jj17arr_w;
    outfile.close();
    infile.open(filepath);
    jj17doc = parse(infile);
    infile.close();

    QVERIFY(holds_alternative<Array>(jj17doc));
    Array& jj17arr = get<Array>(jj17doc);
    QCOMPARE(jj17arr.size(),testList.size());
    QVERIFY(holds_alternative<string>(jj17arr[0]));
    QVERIFY(holds_alternative<string>(jj17arr[1]));
    QVERIFY(holds_alternative<string>(jj17arr[2]));
    QVERIFY(holds_alternative<string>(jj17arr[3]));
    QVERIFY(holds_alternative<string>(jj17arr[4]));
    QVERIFY(holds_alternative<string>(jj17arr[5]));
    QVERIFY(holds_alternative<string>(jj17arr[6]));
    QVERIFY(holds_alternative<string>(jj17arr[7]));
    QVERIFY(holds_alternative<string>(jj17arr[8]));
    QCOMPARE(get<string>(jj17arr[0]), testList[0]);
    QCOMPARE(get<string>(jj17arr[1]), testList[1]);
    QCOMPARE(get<string>(jj17arr[2]), testList[2]);
    QCOMPARE(get<string>(jj17arr[3]), testList[3]);
    QCOMPARE(get<string>(jj17arr[4]), testList[4]);
    QCOMPARE(get<string>(jj17arr[5]), testList[5]);
    QCOMPARE(get<string>(jj17arr[6]), testList[6]);
    QCOMPARE(get<string>(jj17arr[7]), testList[7]);
    QCOMPARE(get<string>(jj17arr[8]), testList[8]);

    dir.removeRecursively();
}

struct MixLatinNum {
    std::string  name{};
    int          depth{0};
    double       coefs[3]{0.,0.,0.};
    MixLatinNum* next{nullptr};
    bool         extention{false};
    bool operator==(const MixLatinNum& oth) const {
        bool coefsOk{true};
        for(unsigned i = 0; i < 3; ++i)
            coefsOk = coefsOk && qFuzzyCompare(coefs[i],oth.coefs[i]);
        return           coefsOk       &&
        (name        ==  oth.name     )&&
        (depth       ==  oth.depth    )&&
        (next        ==  oth.next || *next==*oth.next) &&
        (extention   ==  oth.extention);
    }
    operator QJsonValue() {
        QJsonObject obj;
        obj.insert("name",QString::fromStdString(name));
        obj.insert("depth",depth);
        obj.insert("extention",extention);
        QJsonArray arr;
        for(int i=0;i<3;++i) arr.append(coefs[i]);
        obj.insert("coefs",arr);
        if(next) obj.insert("next",QJsonValue(*next));
        return obj;
    }
    operator jjson17::Value() {
        jjson17::Object obj;
        obj.insert({"name",name});
        obj.insert({"depth",depth});
        obj.insert({"extention",extention});
        jjson17::Array arr;
        for(int i=0;i<3;++i) arr.push_back(coefs[i]);
        obj.insert({"coefs",arr});
        if(next) obj.insert({"next",*next});
        return obj;
    }
    static std::vector<std::unique_ptr<MixLatinNum>>  from(const jjson17::Object& obj)
    {
        using namespace std;
        vector<unique_ptr<MixLatinNum>> results;
        nextObj(obj,results);
        return results;
    }
private:
    static MixLatinNum* nextObj(const jjson17::Object& obj, std::vector<std::unique_ptr<MixLatinNum>>& results)
    {
        using namespace std; using namespace jjson17;
        auto res = make_unique<MixLatinNum>();
        res->name       = get<string> (obj.at("name"));
        res->depth      = get<int64_t>(obj.at("depth"));
        res->extention  = get<bool>   (obj.at("extention"));
        qDebug() << res->name.data() << res->depth;
        const auto& arr = get<Array>(obj.at("coefs"));
        if(arr.size() != 3) throw out_of_range("Wrong len of json array.");
        for(int i=0; i < arr.size(); ++i)
            res->coefs[i] = arr[i];         //using more safe 'operator double()' conversion
        jjson17::Object copyobj = obj;
        if(auto i = obj.find("next"); i!=obj.end()) {
             qDebug() << QString::fromStdString(res->name) << std::distance(obj.begin(),i)<<std::distance(i,obj.end())<<'/'<<obj.size();
             res->next = nextObj(get<Object>(i->second),results);
        }
        results.push_back(std::move(res)); //объекты будут в обратном порядке
        return results.back().get();
    }
};

void QJsonCompatibility::parse_4_mix_latin_nums()
{
    using namespace std;
    using namespace jjson17;

    try {
    //QSKIP("ALREADY COMPLETE");
    vector<MixLatinNum> dep2vec={{"Tetha",2},
                                 {"Omicron",2,{1.,2.,3.},nullptr,true}};
    vector<MixLatinNum> dep1vec={{"Kappa",1,{11.,22.,33.},&dep2vec[0]},
                                 {"Omega",1,{8.,8.,8.},&dep2vec[1]}};
    vector<MixLatinNum> dep0vec={{"Alpha",0,{0.,6.4,66.38}},
                                 {"Beta",0,{0.1,0.2,0.3},&dep1vec[0],true},
                                 {"Gamma",0,{0.,9.01,0.},&dep1vec[1]}};


    QString dirpath  = scopeDirPath+"/parse_4_mix_latin_nums";
    QString filepath = dirpath+"/test_q.json";
    QDir dir("./");
         dir.mkpath(dirpath);
         dir.cd(dirpath);
    QJsonArray qjsarr;
    for(unsigned i =0; i < dep0vec.size(); ++i)
        qjsarr.append(dep0vec[i]);

    QJsonDocument qjsdoc(qjsarr);
    QFile   f(filepath);
            f.open(QIODevice::WriteOnly);
            f.write(qjsdoc.toJson());
            f.close();
    ifstream infile;
             infile.open(filepath.toStdString());
    auto jj17doc = parse(infile);
             infile.close();
    QVERIFY(holds_alternative<Array>(jj17doc));
    Array& jj17arr = get<Array>(jj17doc);
    QCOMPARE(jj17arr.size(),dep0vec.size());
    for(const auto& v : jj17arr)
        qDebug() << QString::fromStdString(to_string(Record("n",v)));
    QVERIFY(holds_alternative<Object>(jj17arr[0]));
    QVERIFY(holds_alternative<Object>(jj17arr[1]));
    QVERIFY(holds_alternative<Object>(jj17arr[2]));

    auto alphaBranch = MixLatinNum::from(get<Object>(jj17arr[0]));
    auto betaBranch  = MixLatinNum::from(get<Object>(jj17arr[1]));
    auto gammaBranch = MixLatinNum::from(get<Object>(jj17arr[2]));

    //back(), так как функция загрузки возвращает вектор в обратном порядке
    QCOMPARE(*(alphaBranch.back()),dep0vec[0]);
    QCOMPARE(*(betaBranch .back()),dep0vec[1]);
    QCOMPARE(*(gammaBranch.back()),dep0vec[2]);
    QVERIFY( !(*(alphaBranch[0])==dep0vec[1]) );
    QVERIFY( !(*(betaBranch [0])==dep0vec[2]) );
    QVERIFY( !(*(gammaBranch[0])==dep0vec[0]) );

    } catch(std::exception& e) {
        qDebug() << "got exception: "<<e.what();
        QFAIL("STD::EXCEPTION");
    } catch(...) {
        qDebug() << "got undef exception: ";
        QFAIL("UNDEF::EXCEPTION");
    }
}
QTEST_APPLESS_MAIN(QJsonCompatibility)

#include "tst_qjsoncompatibility.moc"
