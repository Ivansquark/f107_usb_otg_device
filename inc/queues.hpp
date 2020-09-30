#ifndef QUEUES_HPP
#define QUEUES_HPP

/*! <Simple Static Queue With Const Max Size Must Be Checked On Empty Condition (not optimized) >*/
struct QueWord
{
	QueWord()
	{pThis=this;}
    void push(uint32_t x)
    {
        if(start==nullptr){start = &arr[count]; arr[count]=x;count++;}
        else
        {
            if(count<size)
            {
                for(uint32_t i=count;i>0;i--)
                {arr[i]=arr[i-1];}
                arr[0]=x;
                count++;
            }
        }
    }
    uint32_t pop()
    {
        uint32_t temp=0;
        if(count){count--;} //!< super kostyl'
        if(count)
        {
            temp=arr[count];
            arr[count]=0;
        }
        else
        {
                if (start!=nullptr)
                {
                    temp=arr[0];arr[0]=0;
                    start=nullptr;
                }
                else{/*cout<<"opa";*/}
        }
        return temp;
    }
    bool is_not_empty() //!< this condition must be checked before pop
    {
        //if(start==nullptr)return true;
        //        else return false;
        return (start) ? true : false;
    }    
    uint32_t* start{nullptr};
    uint32_t count{0};
    static constexpr uint32_t size=128; //size of queue
    uint32_t arr[size]{0};
	static QueWord* pThis;
};
QueWord* QueWord::pThis = nullptr;

struct QueByte
{
	QueByte()
	{pThis=this;}
    void push(uint32_t x)
    {
        if(start==nullptr){start = &arr[count]; arr[count]=x;count++;}
        else
        {
            if(count<size)
            {
                for(uint32_t i=count;i>0;i--)
                {arr[i]=arr[i-1];}
                arr[0]=x;
                count++;
            }
        }
    }
    uint32_t pop()
    {
        uint32_t temp=0;
        if(count){count--;} //!< super kostyl'
        if(count)
        {
            temp=arr[count];
            arr[count]=0;
        }
        else
        {
                if (start!=nullptr)
                {
                    temp=arr[0];arr[0]=0;
                    start=nullptr;
                }
                else{/*cout<<"opa";*/}
        }
        return temp;
    }
    bool is_not_empty() //!< this condition must be checked before pop
    {
        //if(start==nullptr)return true;
        //        else return false;
        return (start) ? true : false;
    }    
    uint32_t* start{nullptr};
    uint32_t count{0};
    static constexpr uint32_t size=128; //size of queue
    uint32_t arr[size]{0};
	static QueByte* pThis;
};
QueByte* QueByte::pThis = nullptr;
#endif //QUEUES_HPP