#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/chaniyahzm/shift4";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
        char fpath[1000];
        sprintf(fpath,"%s%s",dirpath,path);
        res = lstat(fpath, stbuf);

        if (res == -1)
                return -errno;

        return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
        char fpath[1000];
        if(strcmp(path,"/") == 0)
        {
                path=dirpath;
                sprintf(fpath,"%s",path);
        }
        else sprintf(fpath, "%s%s",dirpath,path);

        DIR *dp;
        int res;
        struct dirent *de;
//      char tampung[500];

        (void) offset;
        (void) fi;

        dp = opendir(fpath);
        if (dp == NULL)
                return -errno;

        while ((de = readdir(dp)) != NULL) {
                struct stat st;
                memset(&st, 0, sizeof(st));
                st.st_ino = de->d_ino;
                st.st_mode = de->d_type << 12;
                //no 4
//              strcpy(tampung,de->d_name);
//              strcat(tampung,".iz1");
//              if(filler(buf,tampung,&st,0)) break; //klo uda kbuat(1) break
                res = (filler(buf, de->d_name, &st, 0));
                        if(res!=0) break;
        }

        closedir(dp);
        return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
        char fpath[1000];
        if(strcmp(path,"/") == 0)
        {
                path=dirpath;
                sprintf(fpath,"%s",path);
        }
        else sprintf(fpath, "%s%s",dirpath,path);
        int res = 0;
          int fd = 0 ;

        (void) fi;
        fd = open(fpath, O_RDONLY);
        if (fd == -1)
                return -errno;

        res = pread(fd, buf, size, offset);
        if (res == -1)
                res = -errno;

        close(fd);
        return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
        char fpath[1000];
        if(strcmp(path,"/") == 0)
        {
                path=dirpath;
                sprintf(fpath,"%s",path);
        }
        else sprintf(fpath, "%s%s",dirpath,path);
        int res = 0;
        int fd = 0 ;

        char *itu;
        char baru[1000];

        (void) fi;
        fd = open(fpath, O_WRONLY);
        if (fd == -1)
                return -errno;

        res = pwrite(fd, buf, size, offset);
        if (res == -1)
                res = -errno;

        close(fd);
        itu=strstr(fpath, "YOUTUBER");

        sprintf(baru, "%s.iz1", fpath);
        char *pindah[]={"mv",itu,baru,NULL};
        execv("/bin/mv", pindah);

        return res;
}
static int xmp_mkdir(const char *path, mode_t mode)
{

        char fpath[1000];
        if(strcmp(path,"/") == 0)
        {
                path=dirpath;
                sprintf(fpath,"%s",path);
        }
        else sprintf(fpath, "%s%s",dirpath,path);

        int res;
        mode=750;
        res = mkdir(fpath, mode);
        if (res == -1)
                return -errno;

        return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
        char fpath[1000];
        if(strcmp(path,"/") == 0)
        {
                path=dirpath;
                sprintf(fpath,"%s",path);
        }
        else sprintf(fpath, "%s%s",dirpath,path);

        int res;
        mode=640;
        /* On Linux this could just be 'mknod(path, mode, rdev)' but this
           is more portable */
        if (S_ISREG(mode)) {
                res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
                if (res >= 0)
                        res = close(res);
        } else if (S_ISFIFO(mode))
                res = mkfifo(fpath, mode);
        else
                res = mknod(fpath, mode, rdev);
        if (res == -1)
                return -errno;

        return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
        char fpath[500];
        char *ini;
        char *itu;

        if(strcmp(path,"/") == 0)
        {
                path=dirpath;
                sprintf(fpath,"%s",path);
        }
        else sprintf(fpath, "%s%s" ,dirpath,path);

        ini=strrchr(fpath, '.'); //ambil setelah titik
        itu=strstr(fpath,"/YOUTUBER/");  //nyari folder

        if(itu)
        {
                if(strcmp(ini, ".iz1")==0)
                {
                        pid_t pid1;
                        pid1 = fork();
                        if(pid1==0)
                        {
                                char *cr[4]={"zenity","--warning", "--text='File ekstensi iz1 tidak boleh diubah permissionnya.'", NULL};
                                execv("/usr/bin/zenity", cr);
                        }
                }
        }
        return 0;
}

void enkripsi(char inputan[])
{
        int key=17;
        int uk_input = strlen(inputan);
        char print[uk_input];
        char semua[] ="qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
        int a=0;
        int ukuran=sizeof(semua)-1;
        for(a=0 ; a<strlen(inputan) ; a++)
        {
                if(inputan[a] == '/') print[a]='/';
                else
                {
                        char *ambil;
                        int idx;

                        ambil = strchr(semua,inputan[a]);
                        idx=(int)(ambil-semua)%ukuran;

                        int hasil=(idx+key)%ukuran;

                        print[a]=semua[hasil];
                }
        }
}

void dekripsi(char inputan[])
{
        int key=17;
        int uk_input = strlen(inputan);
        char print[uk_input];
        char semua[] ="qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
        int a=0;
        int ukuran=sizeof(semua)-1;
        for(a=0 ; a<strlen(inputan) ; a++)
        {
                if(inputan[a] == '/') print[a]='/';
                else
                {
                        char *ambil;
                        int idx;

                        ambil = strchr(semua,inputan[a]);
                        idx=(int)(ambil-semua)%ukuran;

                        int hasil=(idx-key)%ukuran;

                        print[a]=semua[hasil];
                }
        }
}

static struct fuse_operations xmp_oper = {
        .getattr        = xmp_getattr,
        .readdir        = xmp_readdir,
        .mkdir          = xmp_mkdir,
        .mknod          = xmp_mknod,
        .read           = xmp_read,
        .write          = xmp_write,
        .chmod          = xmp_chmod,
};

int main(int argc, char *argv[])
{
        umask(0);
        return fuse_main(argc, argv, &xmp_oper, NULL);
}
