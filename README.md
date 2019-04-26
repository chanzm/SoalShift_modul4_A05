# SoalShift_modul4_A05

LAPORAN MODUL3 Kelompok A5

Chaniyah Zulfa M (05111740000115)

Faqih Fathan I. (05111740000175)

# SOAL
Suatu ketika King of YouTube ASEAN, Atta Halilintar, mengunjungi laboratorium AJK karena urusan yang benar-benar genting. Prestasi dan ketenarannya ternyata menarik perhatian hacker yang ingin mencuri video-video berkualitas milik Atta, bahkan ingin mencuri password akun YouTube miliknya. <br>

Hacker tersebut menyusup ke dalam file system yang ada di dalam PC Atta dengan mudah karena ternyata file system tersebut sama sekali tidak memiliki sistem pengamanan. Maka dari itu maksud Atta ke laboratorium AJK karena dia ingin kalian untuk mengembangkan file system yang aman.
Atta menginginkan nama file system yang akan kalian buat adalah “AFSHiaAP”, yang nama kepanjangannya adalah AHHA’s File System: Hierarchical and Advanced Protection. Berikut ini kebutuhan yang harus kalian penuhi: <br>

1.) Semua nama file dan folder harus terenkripsi
Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:
``
qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0
``
Misalkan ada file bernama “halo” di dalam folder “INI_FOLDER”, dan key yang dipakai adalah 17, maka:

“INI_FOLDER/halo” saat belum di-mount maka akan bernama “n,nsbZ]wio/QBE#”, 

saat telah di-mount maka akan otomatis terdekripsi kembali menjadi “INI_FOLDER/halo” .

Perhatian: Karakter ‘/’ adalah karakter ilegal dalam penamaan file atau folder dalam *NIX, maka dari itu dapat diabaikan

2.) Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”

   *Urutan operasi dari kebutuhan ini adalah:
      > Tepat saat sebelum file system di-mount
      > Secara otomatis folder “Videos” terbuat di root directory file system
Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file.
      > Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
      > Jangan tampilkan file pecahan di direktori manapun
      
   *Tepat saat file system akan di-unmount
    > Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
    > Hapus folder “Videos” 

3.) Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:

  > Owner Name     : ‘chipset’ atau ‘ic_controller’
  > Group Name    : ‘rusak’
  > Tidak dapat dibaca
  
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.

4.) Pada folder YOUTUBER, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”

5.) Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama Backup kemudian hasil dari save tersebut akan disimpan pada backup dengan nama namafile_[timestamp].ekstensi. Dan ketika file asli dihapus, maka akan dibuat folder bernama RecycleBin, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama namafile_deleted_[timestamp].zip dan ditaruh kedalam folder RecycleBin (file asli dan backup terhapus). Dengan format [timestamp] adalah yyyy-MM-dd_HH:mm:ss

# JAWABAN

```
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
        char *semua;
        int uk_input = strlen(inputan);
        char print[uk_input];
        sprintf(semua, "qE1~ YMUR2%c`hNIdPzi%c^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|%c8s;g<{3.u*W-0", '"', '%', '\\');
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
        char *semua;
        int uk_input = strlen(inputan);
        char print[uk_input];
        sprintf(semua, "qE1~ YMUR2%c`hNIdPzi%c^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|%c8s;g<{3.u*W-0", '"', '%', '\\');
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
```
