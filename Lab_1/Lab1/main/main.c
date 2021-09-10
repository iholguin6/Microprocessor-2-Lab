/*
    Author:     Jesus Minjares
                Master of Science in Computer Engineering
    Course:     EE 5190 Laboratory for Microprocessors Systems II        
    Lab 1:
                Lab 1 goals were to understand the FreeRTOS API, Tasks and Semaphores.
                The goals consisted of creating 4 tasks. 
                Tasks:
                    Task 1: Must run every two seconds
                    Task 2: Must run twice as fast as Task 1
                    Task 3: Must run every time Task 1 or Task 2 are trigger
                    Task 4: Must run every time Task 3 is trigger
                Also, each task must print "Task X running" for each task.
    Date:       09-09-2021

    Pinout:

                                         +-----------------------+
                                         | O      | USB |      O |
                                         |        -------        |
                                     3V3 | [ ]               [ ] | VIN
                                     GND | [ ]               [ ] | GND
     Touch3 / HSPI_CS0 / ADC2_3 / GPIO15 | [ ]               [ ] | GPIO13 / ADC2_4 / HSPI_ID / Touch4
 CS / Touch2 / HSPI_WP / ADC2_2 /  GPIO2 | [ ]               [ ] | GPIO12 / ADC2_5 / HSPI_Q / Touch5
      Touch0 / HSPI_HD / ADC2_0 /  GPIO4 | [ ]               [ ] | GPIO14 / ADC2_6 / HSPI_CLK / Touch6
                         U2_RXD / GPIO16 | [ ]               [ ] | GPIO27 / ADC2_7 / Touch7
                         U2_TXD / GPIO17 | [ ]               [ ] | GPIO26 / ADC2_9 / DAC2
                      V_SPI_CS0 /  GPIO5 | [ ]  ___________  [ ] | GPIO25 / ADC2_8 / DAC1
                SCK / V_SPI_CLK / GPIO18 | [ ] |           | [ ] | GPIO33 / ADC1_5 / Touch8 / XTAL32
        U0_CTS / MSIO / V_SPI_Q / GPIO19 | [ ] |           | [ ] | GPIO32 / ADC1_4 / Touch9 / XTAL32
                 SDA / V_SPI_HD / GPIO21 | [ ] |           | [ ] | GPIO35 / ADC1_7 
                  CLK2 / U0_RXD /  GPIO3 | [ ] |           | [ ] | GPIO34 / ADC1_6 
                  CLK3 / U0_TXD /  GPIO1 | [ ] |           | [ ] | GPIO39 / ADC1_3 / SensVN 
        SCL / U0_RTS / V_SPI_WP / GPIO22 | [ ] |           | [ ] | GPIO36 / ADC1_0 / SensVP 
                MOSI / V_SPI_WP / GPIO23 | [ ] |___________| [ ] | EN 
                                         |                       |
                                         |  |  |  ____  ____  |  |
                                         |  |  |  |  |  |  |  |  |
                                         |  |__|__|  |__|  |__|  |
                                         | O                   O |
                                         +-----------------------+
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
/*Global semaphores*/
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphore2 = NULL;
/*Task 1 will run every 2 seconds*/
void task1(void *pvParameter){
    while(1) {      
        printf("Task 1 running\n"); //display message
        vTaskDelay(2000 / portTICK_RATE_MS); //2 second delay
        xSemaphoreGive( xSemaphore ); //set semaphore flag
    }
}
/*Task 2 will run every  second*/
void task2(void *pvParameter){
    while(1){
        printf("\tTask 2 running\n"); //display message 
        vTaskDelay(1000 / portTICK_RATE_MS); // 1 second delay
        xSemaphoreGive( xSemaphore ); //set semaphore flag
    }
}

/*Task 3 will run once Task 1 or Task 2 finishes it*/
void task3(void *pvParameter){
   while(1){
       /*Wait xSemahpore to be avaiable*/
       if(xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){
            printf("\t\tTask 3 running\n"); //display message 
            vTaskDelay(100 / portTICK_RATE_MS); //small delay to avoid overflow to the buffer
            xSemaphoreGive(xSemaphore); //give back Task1|Task2 semaphore
            xSemaphoreGive(xSemaphore2);  //set Task 4 semaphore
       }
       else{
           vTaskDelay(100/ portTICK_RATE_MS); //delay
       }
   }
}
/*Task 4 will run every time Task 3 runs*/
void task4(void *pvParameter){
   while(1){
       /*Wait xSemahpore to be avaiable*/
       if(xSemaphoreTake(xSemaphore2, portMAX_DELAY) == pdTRUE){
            printf("\t\t\tTask 4 running\n"); //display message 
            vTaskDelay(100 / portTICK_RATE_MS); //small delay to avoid overflow to the buffer
            xSemaphoreGive(xSemaphore2); //give back Task 3 semaphore
       }
       else{
           vTaskDelay(100 / portTICK_RATE_MS); //delay
       }
   }
}
void app_main(){
    /*Intialize global semaphores*/
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphore2 = xSemaphoreCreateBinary();

    /*
    Create Task 1-4, Task 1-2 will have highest priority followed
    by Task 3 and 4 respectively. 
    */
    xTaskCreate(&task1,"task1",4096,(void * const)NULL,5,NULL);
    xTaskCreate(&task2,"task2",4096,(void * const)NULL,5,NULL);
    xTaskCreate(&task3,"task3",4096,(void * const)NULL,6,NULL);
    xTaskCreate(&task4,"task4",4096,(void * const)NULL,7,NULL);
}