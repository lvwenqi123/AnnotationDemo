//
// Created by chengang on 2020/6/22.
//

#ifndef INC_811_INK_FILEPACKAGE_H
#define INC_811_INK_FILEPACKAGE_H

#include "zipconf.h"
#include "zip.h"
#include "LogUtil.h"
#include "FileFunc.h"
#define tag "Package"

struct FilePackage{
protected:
    static int CollectFile2Zip(const char *path, const int rootDirLen, struct zip * zipfile)
    {
        DIR *dir;
        dirent *dir_info;
        char file_path[PATH_MAX];
        if(is_file(path)) {
            return Add2Zip(zipfile, path, rootDirLen);
        }
        if(is_dir(path)) {
            if((dir = opendir(path)) == nullptr)
                return 0;
            while((dir_info = readdir(dir)) != nullptr) {
                get_file_path(path, dir_info->d_name, file_path);
                if(is_special_dir(dir_info->d_name))
                    continue;
                if (CollectFile2Zip(file_path, rootDirLen, zipfile) == -1) {
                    return -1;
                }
            }
        }
        return 0;
    }

    static int Add2Zip(struct zip * zipfile, const char *filePath, const int  rootDirLen) {
        struct zip_source *srcfile = zip_source_file(zipfile, filePath, 0, -1);
        if (!srcfile) {
            LOG_E(tag, "open source file failed");
            zip_close(zipfile);
            return -1;
        }
        std::string dst = Str::splitBySize(filePath, rootDirLen);
        //LOG_D(tag, "dst = %s", dst.c_str());
        zip_file_add(zipfile, dst.c_str(), srcfile, ZIP_FL_OVERWRITE);
        return 0;
    }
    static std::vector<std::string> GetDir(const std::string str)
    {
        std::string::size_type pos;
        std::string pattern = "/";
        std::vector<std::string> result;
        int size = str.size();
        for(int i = 0; i < size; i++)
        {
            pos = str.find(pattern,i);
            if (pos < size) {
                std::string s = str.substr(i, pos - i);
                result.push_back(s);
                i=pos+pattern.size() - 1;
            }
        }
        return result;
    }

public:
    /*压缩文件*/
    static void ZipFile(std::string &zipFileName, std::string &srcPath) {
        LOG_E(tag, "zipFileName = %s srcPath = %s", zipFileName.c_str(), srcPath.c_str());
        int iErr = 0;
        struct zip *zipfile = NULL;
        struct zip_source *srcfile = NULL;

        zipfile = zip_open(zipFileName.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &iErr);
        if (!zipfile) {
            LOG_E(tag, "zip open failed err = %d", iErr);
            return;
        }
        if (CollectFile2Zip(srcPath.c_str(), srcPath.size(), zipfile) == 0) {
            zip_close(zipfile);
        } else {
            LOG_D(tag, "doCompress Error");
        }
    }
    /*解压文件*/

    static std::string UnZipFile(std::string &zipFileName, std::string &iwbDir)
    {
        LOG_D(tag, "zipFileName = %s", zipFileName.c_str());
        std::string path = iwbDir + "tmp/";
        if (0 == access(path.c_str(), F_OK)) {
            delete_file(path.c_str());
        }
        if (0 != mkdir(path.c_str(), 0755)) {
            DBG("CreateDir path = %s failed", path.c_str());
        }
        int iErr = 0;
        struct zip * zipfile = NULL;
        struct zip_file *entries = NULL;
        struct zip_stat stat;
        zip_int64_t i64Num = 0;
        zip_int64_t i64Count = 0;
        int iRead = 0;
        int iLen = 0;
        char buf[1024];

        memset(&stat, 0, sizeof(stat));
        memset(buf, 0, sizeof(buf));

        zipfile = zip_open(zipFileName.c_str(), ZIP_CHECKCONS, &iErr);
        if(!zipfile) {
            LOG_D(tag, "zip open failed err = :%d", iErr);
            return "";
        }

        //get how many entrrites in archive
        i64Num = zip_get_num_entries(zipfile, 0);

        for (i64Count = 0; i64Count < i64Num; i64Count++)
        {
            if (zip_stat_index(zipfile, i64Count, 0, &stat) == 0)
            {
                LOG_D(tag, "the file name is:%s", stat.name);
            }

            entries = zip_fopen_index(zipfile, i64Count, 0);
            if (!entries)
            {
                LOG_D(tag, "fopen index failed");
                goto End;
            }

            //create the original file
            std::string fileName = path + stat.name;
            FILE *fp = fopen(fileName.c_str(), "w+");
            LOG_D(tag, "fileName = %s", fileName.c_str());
            if (!fp) {
                StrVec dirVec = GetDir(stat.name);
                if (!dirVec.empty()) {
                    std::string dir = path;
                    for (int i = 0 ;i < dirVec.size();++i) {
                        dir += dirVec[i];
                        mkdir(dir.c_str(), 0755);
                        dir += "/";
                    }
                }
                fp = fopen(fileName.c_str(), "w+");
                if (!fp) {
                    LOG_D(tag, "create local file failed fileName = %s", fileName.c_str());
                    goto End;
                }
            }
            while(iLen < stat.size) {
                iRead = zip_fread(entries, buf, 1024);
                if (iRead < 0) {
                    LOG_D(tag, "read file failed");
                    fclose(fp);
                    goto End;
                }

                fwrite(buf, 1, iRead, fp);
                iLen += iRead;
            }
            fclose(fp);
            iLen = 0;
            iRead = 0;
        }
        End:
        zip_close(zipfile);
        return path;
    }
};

#endif //INC_811_INK_FILEPACKAGE_H
