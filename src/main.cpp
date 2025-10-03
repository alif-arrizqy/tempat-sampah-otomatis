/*
 * Tempat Sampah Otomatis
 * Proyek ini menggunakan sensor ultrasonik HC-SR04 dan servo motor
 * untuk membuat tempat sampah yang membuka otomatis ketika ada objek mendekat
 */

#include <Arduino.h>
#include <NewPing.h>
#include <Servo.h>

// Pin dan konstanta
#define TRIGGER_PIN  12     // Pin trigger sensor ultrasonik
#define ECHO_PIN     11     // Pin echo sensor ultrasonik
#define SERVO_PIN    9      // Pin kontrol servo motor
#define MAX_DISTANCE 200    // Jarak maksimum sensor (cm)
#define JARAK_DETEKSI 10    // Jarak untuk membuka tutup (cm)
#define SERVO_TUTUP  0      // Posisi servo tutup (derajat)
#define SERVO_BUKA   90     // Posisi servo buka (derajat)

// Objek
Servo servoTutup;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // Sistem initialization
  Serial.begin(9600);
  servoTutup.attach(SERVO_PIN);   // Hubungkan servo ke pin
  servoTutup.write(SERVO_TUTUP);  // Posisi awal: tutup
  
  Serial.println("=== Tempat Sampah Otomatis ===");
  Serial.println("Sistem siap beroperasi...");
  delay(2000);
}

// Fungsi untuk membaca jarak dengan filtering yang lebih baik
int bacaJarakDenganFilter() {
  int pembacaan[5];
  int validCount = 0;
  
  // Baca sensor 5 kali dengan jeda yang cukup
  for (int i = 0; i < 5; i++) {
    delay(60); // Delay minimum untuk sensor ultrasonik
    int jarak = sonar.ping_cm();
    
    // Filter nilai yang masuk akal (antara 2-200 cm)
    if (jarak >= 2 && jarak <= 200) {
      pembacaan[validCount] = jarak;
      validCount++;
    }
  }
  
  // Jika tidak ada pembacaan valid, return 0
  if (validCount == 0) {
    return 0;
  }
  
  // Hitung rata-rata dari pembacaan yang valid
  int total = 0;
  for (int i = 0; i < validCount; i++) {
    total += pembacaan[i];
  }
  
  return total / validCount;
}

void loop() {
  // Scan jarak dengan filtering yang lebih baik
  Serial.print("Scanning... ");
  int jarak = bacaJarakDenganFilter();
  
  Serial.print("Jarak: ");
  if (jarak == 0) {
    Serial.println("Di luar jangkauan atau jarak > 200 cm");
  } else {
    Serial.print(jarak);
    Serial.println(" cm");
  }
  
  // Jika ada objek mendekat dengan jarak <= 10 cm
  if (jarak > 0 && jarak <= JARAK_DETEKSI) {
    Serial.println(">>> Objek terdeteksi! Membuka tutup...");
    
    // Matikan servo signal untuk mengurangi interferensi
    servoTutup.detach();
    delay(100);
    
    // Nyalakan kembali dan buka tutup
    servoTutup.attach(SERVO_PIN);
    servoTutup.write(SERVO_BUKA);
    Serial.println(">>> Tutup TERBUKA - menunggu 10 detik...");
    
    // Tunggu servo mencapai posisi (tidak terburu-buru)
    delay(1000);
    
    // Matikan servo lagi untuk mengurangi noise selama delay
    servoTutup.detach();
    
    // Countdown 10 detik tanpa interferensi servo
    for (int i = 10; i > 0; i--) {
      Serial.print("Tutup dalam: ");
      Serial.print(i);
      Serial.println(" detik");
      delay(1000);
    }
    
    // Nyalakan servo dan tutup
    Serial.println(">>> Menutup tutup...");
    servoTutup.attach(SERVO_PIN);
    servoTutup.write(SERVO_TUTUP);
    
    // Tunggu servo menutup
    delay(1000);
    
    // Matikan servo lagi
    servoTutup.detach();
    
    Serial.println(">>> Tutup TERTUTUP");
    Serial.println("---");
    
    // Stabilisasi sebelum scan berikutnya
    delay(2000);
    
    // Nyalakan servo kembali untuk operasi normal
    servoTutup.attach(SERVO_PIN);
  }
  
  // Delay scan berikutnya (total sekitar 3 detik termasuk waktu pembacaan)
  delay(2500);
}
