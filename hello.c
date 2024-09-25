#include <stdio.h>

// Định nghĩa typedef struct
typedef struct student
{
    char name[20];
    int eng;
    int math;
    int phys;
    double mean;
} STUDENT;

// Mảng dữ liệu sinh viên
STUDENT data[] = {
    {"Tuan", 82, 72, 58, 0.0},
    {"Nam", 77, 82, 79, 0.0},
    {"Khanh", 52, 62, 39, 0.0},
    {"Phuong", 61, 82, 88, 0.0}};

// Hàm tính điểm trung bình và ghi vào mảng
void calculate_mean(STUDENT *s)
{
    s->mean = (s->eng + s->math + s->phys) / 3.0;
}

// Hàm trả về hạng dựa trên điểm trung bình
char get_grade(double mean)
{
    if (mean >= 90 && mean <= 100)
        return 'S';
    else if (mean >= 80 && mean < 90)
        return 'A';
    else if (mean >= 70 && mean < 80)
        return 'B';
    else if (mean >= 60 && mean < 70)
        return 'C';
    else
        return 'D';
}

// Hàm in kết quả và hạng ra màn hình
void print_students_with_pointer(STUDENT *p, int size)
{
    for (int i = 0; i < size; i++, p++)
    {
        printf("Student: %s, Mean: %.2f, Grade: %c\n",
               p->name, p->mean, get_grade(p->mean));
    }
}

int main()
{
    int size = sizeof(data) / sizeof(data[0]);
    STUDENT *p = data; // Con trỏ p trỏ tới mảng data

    // Tính điểm trung bình cho từng sinh viên
    for (int i = 0; i < size; i++)
    {
        calculate_mean(&data[i]);
    }

    // In kết quả và hạng ra màn hình dùng con trỏ
    print_students_with_pointer(p, size);

    return 0;
}
