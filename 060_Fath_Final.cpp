#include <iostream>
#include <fstream>

using namespace std;

struct DetailPesanan {
    char nama[50];
    int jumlah;
    int subtotal;
    DetailPesanan* next;
};

struct Menu {
    char nama[50];
    char kategori[20];
    int harga;
    Menu* terkait[3];
    int jumlahTerkait;
    char terkaitNama[3][50];
    Menu* next;
};

struct Pesanan {
    int nomor;
    int total;
    bool selesai;
    DetailPesanan* detail;
    Pesanan* next;
};

struct StackNode {
    Pesanan data;
    StackNode* next;
};

struct TreeNode {
    Menu* data;
    TreeNode* left;
    TreeNode* right;
};

struct HashNode {
    Menu* data;
    HashNode* next;
};

HashNode* kategoriTable[3];
Pesanan* frontQueue = NULL;
Pesanan* rearQueue = NULL;
StackNode* topStack = NULL;
TreeNode* rootBST = NULL;

bool adminMode = false;
int nomorPesanan = 1;

void copyString(char tujuan[], char sumber[]) {
    int i = 0;

    while (sumber[i] != '\0') {
        tujuan[i] = sumber[i];
        i++;
    }
    tujuan[i] = '\0';
}

bool samaString(char a[], char b[]) {
    int i = 0;

    while (a[i] != '\0' || b[i] != '\0') {
        if (a[i] != b[i])
            return false;
        i++;
    }
    return true;
}

int stringKeInt(char teks[]) {
    int hasil = 0;
    int i = 0;

    while (teks[i] != '\0') {
        hasil = hasil * 10 + (teks[i] - '0');
        i++;
    }
    return hasil;
}

void ambilData(char sumber[], char tujuan[], int& index) {
    int i = 0;

    while (sumber[index] != '|' && sumber[index] != '\0') {
        tujuan[i] = sumber[index];
        i++;
        index++;
    }
    tujuan[i] = '\0';

    if (sumber[index] == '|')
        index++;
}

int hashKategori(char kategori[]) {
    if (samaString(kategori, (char*)"Makanan"))
        return 0;

    else if (samaString(kategori, (char*)"Minuman"))
        return 1;

    return 2;
}

void insertKategori(Menu* menuBaru) {
    int index = hashKategori(menuBaru->kategori);

    HashNode* baru = new HashNode;

    baru->data = menuBaru;
    baru->next = kategoriTable[index];
    kategoriTable[index] = baru;
}

TreeNode* insertBST(TreeNode* root, Menu* data) {
    if (root == NULL) {
        TreeNode* baru = new TreeNode;

        baru->data = data;

        baru->left = NULL;
        baru->right = NULL;
        return baru;
    }

    if (data->harga < root->data->harga)
        root->left = insertBST(root->left, data);

    else
        root->right = insertBST(root->right, data);

    return root;
}

void inorderBST(TreeNode* root) {
    if (root == NULL)
        return;

    inorderBST(root->left);

    cout << root->data->nama << " | Rp." << root->data->harga << endl;

    inorderBST(root->right);
}

Menu* cariMenuGlobal(char nama[]) {
    for (int i = 0; i < 3; i++) {
        HashNode* temp = kategoriTable[i];

        while (temp != NULL) {
            if (samaString( temp->data->nama, nama)) {
                return temp->data;
            }
            temp = temp->next;
        }
    }
    return NULL;
}

void tampilRecursive(HashNode* temp) {
    if (temp == NULL)
        return;

    cout << "- " << temp->data->nama << " | Rp." << temp->data->harga << endl;

    if (temp->data->jumlahTerkait > 0) {
        cout << "  Rekomendasi : ";
        for (int i = 0; i < temp->data->jumlahTerkait; i++) {
            if (temp->data->terkait[i] != NULL) {
                cout << temp->data
                           ->terkait[i]
                           ->nama;
                if (i != temp ->data ->jumlahTerkait - 1)
                    cout << ", ";
            }
        }
        cout << endl;
    }
    cout << endl;

    tampilRecursive(temp->next);
}
void loadTerkait(Menu* menu, char teks[]);

void loadMenu() {
    ifstream file("menu.txt");

    if (!file.is_open())
        return;

    while (!file.eof()) {
        char baris[200];

        file.getline(baris, 200);

        if (baris[0] == '\0')
            continue;

        Menu* baru = new Menu;

        int index = 0;
        char hargaText[20];
        char terkaitText[100];

        ambilData(baris, baru->nama, index);

        ambilData(baris, baru->kategori, index);

        ambilData(baris, hargaText, index);

        baru->harga = stringKeInt(hargaText);

        ambilData(baris, terkaitText, index);

        loadTerkait(baru, terkaitText);

        baru->jumlahTerkait = 0;

        baru->next = NULL;

        insertKategori(baru);

        rootBST = insertBST(rootBST, baru);
    }
    file.close();
}

void hubungkanGraph() {
    for (int i = 0; i < 3; i++) {
        HashNode* temp = kategoriTable[i];

        while (temp != NULL) {
            Menu* menu = temp->data;

            for (int j = 0; j < menu->jumlahTerkait; j++) {
                menu->terkait[j] =
                    cariMenuGlobal(
                        menu->terkaitNama[j]
                    );
            }
            temp = temp->next;
        }
    }
}

void loadAntrean() {
    ifstream file("antrian.txt");
    if (!file.is_open())
        return;

    Pesanan* transaksi = NULL;

    while (!file.eof()) {
        char baris[200];

        file.getline(baris, 200);

        if (baris[0] == '\0')
            continue;

        if (baris[0] == 'P') {
            transaksi = new Pesanan;

            transaksi->detail = NULL;

            transaksi->next = NULL;

            transaksi->selesai = false;

            transaksi->total = 0;

            if (rearQueue == NULL) {
                frontQueue = rearQueue = transaksi;
            }

            else {
                rearQueue->next = transaksi;

                rearQueue = transaksi;
            }
        }

        else if (baris[0] == 'T') {
            char totalText[20];
            int i = 8;
            int j = 0;

            while (baris[i] != '\0') {
                totalText[j] = baris[i];
                i++;
                j++;
            }
            totalText[j] = '\0';
            transaksi->total = stringKeInt(totalText);
        }

        else {
            DetailPesanan* detail = new DetailPesanan;

            int index = 0;
            char jumlahText[20];
            char subtotalText[20];

            ambilData(baris, detail->nama, index);

            ambilData(baris, jumlahText, index);

            ambilData(baris, subtotalText, index);

            detail->jumlah = stringKeInt(jumlahText);

            detail->subtotal = stringKeInt(subtotalText);

            detail->next = NULL;

            if (transaksi->detail == NULL)
                transaksi->detail = detail;

            else {
                DetailPesanan* akhir = transaksi->detail;

                while (akhir->next != NULL)
                    akhir = akhir->next;

                akhir->next = detail;
            }
        }
    }
    file.close();
}

void loadRiwayat() {
    ifstream file("riwayat.txt");

    if (!file.is_open())
        return;

    while (!file.eof()) {
        char baris[200];

        file.getline(baris, 200);

        if (baris[0] == 'P') {
            Pesanan data;

            data.selesai = true;

            StackNode* baru =  new StackNode;

            baru->data = data;

            baru->next = topStack;

            topStack = baru;
        }
    }
    file.close();
}

void simpanAntrean() {
    ofstream file("antrian.txt");
    Pesanan* temp = frontQueue;

    while (temp != NULL) {
        file << "Pesanan " << temp->nomor << " (belum selesai)" << endl;

        DetailPesanan* detail = temp->detail;

        while (detail != NULL) {
            file << detail->nama << "|" << detail->jumlah << "|" << detail->subtotal << endl;
            detail = detail->next;
        }
        file << "Total = " << temp->total << endl << endl;

        temp = temp->next;
    }
    file.close();
}

int hitungRiwayat() {
    ifstream file("riwayat.txt");
    if (!file.is_open())
        return 0;

    char baris[200];

    int jumlah = 0;

    while (!file.eof()) {
        file.getline(baris, 200);
        if (baris[0] == 'P')
            jumlah++;
    }
    file.close();

    return jumlah;
}

void simpanRiwayat(Pesanan* data) {
    ofstream file("riwayat.txt", ios::app);

    int nomorRiwayat = hitungRiwayat() + 1;

    file << "Pesanan " << nomorRiwayat << " (selesai)" << endl;

    DetailPesanan* detail =
        data->detail;

    while (detail != NULL) {
        file << detail->nama << "|" << detail->jumlah << "|" << detail->subtotal << endl;

        detail = detail->next;
    }
    file << "Total = " << data->total << endl << endl;

    file.close();
}

Menu* cariMenuKategori(int kategori, char nama[]) {
    HashNode* temp =
        kategoriTable[kategori];

    while (temp != NULL) {
        if (samaString(
                temp->data->nama, nama))
            return temp->data;
        temp = temp->next;
    }
    return NULL;
}

void updateNomorPesanan() {
    Pesanan* temp = frontQueue;

    int nomor = 1;

    while (temp != NULL) {
        temp->nomor = nomor;

        nomor++;

        temp = temp->next;
    }
    nomorPesanan = nomor;
}

void tambahMenu() {
    Menu* baru = new Menu;

    int kategori;

    cout << "\n===== TAMBAH MENU =====\n";
    cout << "1. Makanan\n";
    cout << "2. Minuman\n";
    cout << "3. Snack\n";
    cout << "Pilih kategori : ";

    cin >> kategori;

    cin.ignore();

    if (kategori == 1)
        copyString( baru->kategori, (char*)"Makanan");

    else if (kategori == 2)
        copyString( baru->kategori,(char*)"Minuman");

    else
        copyString( baru->kategori, (char*)"Snack"
        );

    cout << "Nama menu : ";
    cin.getline(baru->nama, 50);

    cout << "Harga : ";
    cin >> baru->harga;
    cin.ignore();

    cout << "Jumlah menu terkait : ";
    cin >> baru->jumlahTerkait;
    cin.ignore();

    if (baru->jumlahTerkait > 3)
        baru->jumlahTerkait = 3;

    for (int i = 0; i < baru->jumlahTerkait; i++) {
        char namaTerkait[50];

        cout << "Menu terkait ke-" << i + 1 << " : ";

        cin.getline(namaTerkait, 50);
        
        copyString( baru->terkaitNama[i], namaTerkait);

        Menu* menuTerkait = cariMenuGlobal(namaTerkait);

        if (menuTerkait != NULL)
            baru->terkait[i] = menuTerkait;

        else {
            baru->terkait[i] = NULL;
            cout << "Menu tidak ditemukan!\n";
        }
    }
    baru->next = NULL;

    insertKategori(baru);

    rootBST = insertBST(rootBST, baru);

    ofstream file("menu.txt", ios::app);

    file << baru->nama << "|" << baru->kategori << "|" << baru->harga << "|";

for (int i = 0; i < baru->jumlahTerkait; i++) {

    file << baru->terkait[i]->nama;

    if (i != baru->jumlahTerkait - 1)
        file << ",";
}
file << endl;

    file.close();

    cout << "\nMenu berhasil ditambahkan!\n";
}

void loadTerkait(Menu* menu, char teks[]) {
    int i = 0;
    int j = 0;
    int index = 0;
    char nama[50];
    while (true) {
        if (teks[i] == ',' || teks[i] == '\0') {
            nama[j] = '\0';
            if (j > 0) {
                copyString( menu->terkaitNama[index], nama);
                index++;
            }
            j = 0;
            if (teks[i] == '\0')
                break;
        }
        else {
            nama[j] = teks[i];
            j++;
        }
        i++;
    }
    menu->jumlahTerkait = index;
}

void tampilMenu() {
    int pilih;

    cout << "\n===== DAFTAR MENU =====\n";
    cout << "1. Makanan\n";
    cout << "2. Minuman\n";
    cout << "3. Snack\n";
    cout << "Pilih : ";
    cin >> pilih;

    if (pilih < 1 || pilih > 3) {
        cout << "\nKategori salah!\n";
        return;
    }

    HashNode* temp = kategoriTable[pilih - 1];

    tampilRecursive(temp);
}

void searchingMenu() {
    char cari[50];

    cin.ignore();
    cout << "\nCari menu : ";
    cin.getline(cari, 50);

    bool ditemukan = false;

    for (int i = 0; i < 3; i++) {
        HashNode* temp = kategoriTable[i];

        while (temp != NULL) {
            if (samaString(temp->data->nama, cari)) {
                cout << "\nMenu ditemukan!\n";
                cout << temp->data->nama << " | Rp." << temp->data->harga << endl;
                ditemukan = true;
            }
            temp = temp->next;
        }
    }

    if (!ditemukan)
        cout << "\nMenu tidak ditemukan!\n";
}

void pesanMakanan() {
    Pesanan* transaksi = new Pesanan;

    transaksi->nomor = nomorPesanan++;

    transaksi->total = 0;

    transaksi->selesai = false;

    transaksi->detail = NULL;

    transaksi->next = NULL;

    int lanjut = 1;

    while (lanjut == 1) {
        int kategori;
        cout << "\n===== PILIH KATEGORI =====\n";
        cout << "1. Makanan\n";
        cout << "2. Minuman\n";
        cout << "3. Snack\n";
        cout << "Pilih : ";
        cin >> kategori;
        cin.ignore();

        HashNode* temp = kategoriTable[kategori - 1];

        tampilRecursive(temp);

        char namaMenu[50];

        cout << "Pilih menu : ";
        cin.getline(namaMenu, 50);
        
        Menu* menuDipilih = cariMenuKategori(kategori - 1, namaMenu);

        if (menuDipilih == NULL) {

            cout << "\nMenu tidak ditemukan!\n";
            return;
        }

        int jumlah;
        cout << "Jumlah : ";
        cin >> jumlah;

        DetailPesanan* detailBaru = new DetailPesanan;

        copyString(detailBaru->nama, menuDipilih->nama);

        detailBaru->jumlah = jumlah;

        detailBaru->subtotal = jumlah *menuDipilih->harga;

        detailBaru->next = NULL;

        if (transaksi->detail == NULL)
            transaksi->detail = detailBaru;

        else {
            DetailPesanan* akhir =
                transaksi->detail;

            while (akhir->next != NULL)
                akhir = akhir->next;

            akhir->next = detailBaru;
        }

        transaksi->total += detailBaru->subtotal;
        cout << "\n1. Tambah lagi\n";
        cout << "2. Selesai\n";
        cout << "Pilih : ";
        cin >> lanjut;
    }

    if (rearQueue == NULL) {
        frontQueue = rearQueue = transaksi;
    }

    else {
        rearQueue->next = transaksi;

        rearQueue = transaksi;
    }

    StackNode* stackBaru = new StackNode;

    stackBaru->data = *transaksi;

    stackBaru->next = topStack;

    topStack = stackBaru;

    simpanAntrean();

    cout << "\n===== TOTAL =====\n";

    DetailPesanan* detail = transaksi->detail;

    while (detail != NULL) {
        cout << detail->nama << " x" << detail->jumlah << " = Rp." << detail->subtotal << endl;

        detail = detail->next;
    }

    cout << "Total = Rp." << transaksi->total << endl;
}

void antreanPesanan() {
    if (frontQueue == NULL) {
        cout << "\nAntrean kosong!\n";
        return;
    }

    Pesanan* temp = frontQueue;

    cout << "\n===== ANTREAN =====\n";

    while (temp != NULL) {
        cout << "\nPesanan " << temp->nomor << endl;

        DetailPesanan* detail =
            temp->detail;

        while (detail != NULL) {
            cout << detail->nama << "|" << detail->jumlah << "|" << detail->subtotal << endl;
            detail = detail->next;
        }

        cout << "Total = " << temp->total << endl;
        temp = temp->next;
    }
}

void selesaikanPesanan() {
    if (frontQueue == NULL) {
        cout << "\nAntrean kosong!\n";
        return;
    }

    Pesanan* selesai = frontQueue;

    selesai->selesai = true;

    simpanRiwayat(selesai);

    frontQueue = frontQueue->next;

    if (frontQueue == NULL)
        rearQueue = NULL;

    delete selesai;

    updateNomorPesanan();

    simpanAntrean();

    cout << "\nPesanan selesai!\n";
}

void tampilRiwayat() {
    ifstream file("riwayat.txt");

    if (!file.is_open()) {
        cout << "\nRiwayat kosong!\n";
        return;
    }

    char baris[200];

    cout << "\n===== RIWAYAT =====\n";

    while (!file.eof()) {

        file.getline(baris, 200);

        if (baris[0] != '\0')
            cout << baris << endl;
    }

    file.close();
}

void batalkanPesanan() {
    if (frontQueue == NULL) {
        cout << "\nTidak ada antrean!\n";
        return;
    }

    cout << "\n===== LIST PESANAN =====\n";

    Pesanan* lihat = frontQueue;

    while (lihat != NULL) {
        cout << "- Pesanan " << lihat->nomor << endl;

        lihat = lihat->next;
    }

    int nomor;
    cout << "\nNomor pesanan : ";
    cin >> nomor;

    Pesanan* temp = frontQueue;

    Pesanan* prev = NULL;

    while (temp != NULL) {
        if (temp->nomor == nomor) {
            if (prev == NULL)
                frontQueue = temp->next;

            else
                prev->next = temp->next;

            if (temp == rearQueue)
                rearQueue = prev;

            delete temp;
            updateNomorPesanan();
            simpanAntrean();

            cout << "\nPesanan dibatalkan!\n";
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    cout << "\nPesanan tidak ditemukan!\n";
}

void loginAdmin() {
    char password[20];

    cin.ignore();
    cout << "\nPassword : ";
    cin.getline(password, 20);

    char benar[] = "admin123";

    if (samaString(password, benar)) {
        adminMode = true;
        cout << "\nMODE ADMIN AKTIF!\n";
    }
    else
        cout << "\nPassword salah!\n";
}

int main() {
    loadMenu();

    hubungkanGraph();

    loadAntrean();

    loadRiwayat();

    int pilih;

    do {
        cout << "\n===== SISTEM RESTORAN =====\n";
        cout << "1. Daftar Menu\n";
        cout << "2. Pesan Makanan\n";
        cout << "3. Cari Menu\n";
        cout << "4. Urutkan Harga\n";

        if (adminMode) {
            cout << "5. Tambah Menu\n";
            cout << "6. Antrean Pesanan\n";
            cout << "7. Selesaikan Pesanan\n";
            cout << "8. Riwayat Pesanan\n";
            cout << "9. Batalkan Pesanan\n";
        }
        cout << "99. Keluar\n";

        cout << "Pilih : ";
        cin >> pilih;
        switch (pilih) {

        case 0:
            loginAdmin();
            break;

        case 1:
            tampilMenu();
            break;

        case 2:
            pesanMakanan();
            break;

        case 3:
            searchingMenu();
            break;

        case 4:
            cout << "\n===== SORT BST =====\n";
            inorderBST(rootBST);
            break;

        case 5:
            if (adminMode)
                tambahMenu();
            break;

        case 6:
            if (adminMode)
                antreanPesanan();
            break;

        case 7:
            if (adminMode)
                selesaikanPesanan();
            break;

        case 8:
            if (adminMode)
                tampilRiwayat();
            break;

        case 9:
            if (adminMode)
                batalkanPesanan();
            break;

        case 99:
            cout << "\nProgram selesai!\n";
            break;

        default:
            cout << "\nPilihan salah!\n";
        }
    } while (pilih != 99);

    return 0;
}