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
    void perf_1_dirscan();

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
    jjson17::Array  w_arr{ val1,
                           val2,
                           val3,
                           val4
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
static jjson17::Object subScanFunc (std::filesystem::directory_entry entry, int depth)
{
    namespace json = jjson17;
    namespace fs = std::filesystem;

    static const int MAX_DEPTH = 2;
    json::Object obj {
        {"Directory",entry.is_directory()},
        {"Size",entry.file_size()},
        {"Permitions",static_cast<int>(entry.status().permissions())}
    };

    if(entry.is_directory() && depth < MAX_DEPTH)
    {
        depth++;
        json::Object content;
        for(const auto& d : fs::directory_iterator(entry,fs::directory_options::skip_permission_denied))
            content.insert( {d.path().filename().generic_string() , subScanFunc(d,depth)} );//content.insert( {d.path().filename().u8string() , subScanFunc(d,depth)} );
        obj.insert({"content",content});
    }
    return obj;
}
//recursive dirscan
static QJsonObject subScanFunc (const QFileInfo& entry, int depth)
{
    static const int MAX_DEPTH = 2;
    QJsonObject obj {
        {"Directory",entry.isDir()},
        {"Size",entry.size()},
        {"Permitions",static_cast<int>(entry.permissions())}
    };

    if(entry.isDir() && depth < MAX_DEPTH)
    {
        depth++;
        QJsonObject content;
        foreach(const auto& d , QDir(entry.filePath()).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries))
            content.insert(d.fileName(),subScanFunc(d,depth));
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

QTEST_APPLESS_MAIN(QJsonCompatibility)

#include "tst_qjsoncompatibility.moc"
