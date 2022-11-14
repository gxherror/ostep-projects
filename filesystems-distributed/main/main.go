
package main


//需求: 利用Go高并发秒级处理10万数据


import(

 "fmt"

 _"time"

)


//生产消息

func putNum(intChan chan int, nums int) {

 for i:=0; i< nums; i++{

 intChan<- i

 //fmt.Printf("==putNum:%d==\n", i)

 }

 close(intChan)

}


//消费消息

func primeNum(intChan chan int, primeChan chan int, exitChan chan bool, chanNum int) {

 var flg bool

 for{

 //time.Sleep(time.Second)

 //取数据

 num, ok := <- intChan

 //判断管道是否无数据可取

 if !ok{

 break

 }

 //判断是否素数(除了1和本身可以整除的数),默认标识非素数

 flg = false

 for i:=2; i<num; i++{

 if num % i != 0{

 flg = true

 break

 }

 }

 //将素数放进管道2

 if flg{

 primeChan<- num

 }

 }

 fmt.Printf("==协程%d完成作业，退出==\n", chanNum)

 //向退出管道告知，我这个协程完成作业了，写入一个T

 exitChan<- true

}


func main() {

 //创建管道1：存数据

 intChan := make(chan int, 200000)

 //创建管道2：存处理后的数据

 primeChan := make(chan int, 2000) 

 //创建管道3：存管道处理完成的标识

 exitChan := make(chan bool, 4) 


 //开启一个协程 向管道1写数据

 go putNum(intChan, 100000)

 //time.Sleep(10 * time.Second)


 //开启4个协程从管道1取数据并且判断是否素数，将素数存入管道2中,另外取不到数据时将T写进管道3

 for i:=1; i<=4; i++{

 go primeNum(intChan, primeChan, exitChan, i)

 } 

 //处理主线程阻塞

 go func () {

 //只要能取出4个数就认为管道都完成作业，此时主线程可以退出

 for i:=1; i<=4; i++{

 <-exitChan

 } 

 close(primeChan) 

 }()


 //遍历出渠道2中的素数

 for {

 _,ok := <-primeChan

 if !ok{

 break

 }

 //fmt.Printf("==素数:%d==\n", res)

 }


 fmt.Println("==主线程退出==")

}
