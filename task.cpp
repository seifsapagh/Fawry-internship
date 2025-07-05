#include <iostream>
using namespace std;
#include <vector>
#include <stdexcept>

string getCurrentDate(){
    return "2025-07-10";
}
class Shippable{
public:
    virtual double getWeight() const = 0;
    virtual ~Shippable() {}
};

class Expirable{
public:
    virtual bool isExpired() const = 0;
    virtual string getExpiryDate() const = 0;
    virtual ~Expirable() {}
};

class ShippingInfo: public Shippable{
    double weight;
public:
    ShippingInfo(double w): weight(w) {}
    double getWeight()  const override{ return weight; }
};

class ExpiryInfo: public Expirable{
    string expiryDate;
public:
    ExpiryInfo(string e): expiryDate(e) {}
    string getExpiryDate() const override {return expiryDate;}
    bool isExpired() const override{
        return getCurrentDate() > expiryDate;
    }
};

class Product{
    string name;
    float price;
    int quantity;
public:
    Shippable* shippingInfo = nullptr;
    Expirable* expiryInfo = nullptr;

    Product(string name, float price, int quantity){
        this->name = name;
        this->price = price;
        this->quantity = quantity;
    }

    string getName(){
        return name;
    }

    float getPrice(){
        return price;
    }

    int getQuantity(){
        return quantity;
    }

    void updateQuantity(int newQuantity){
        quantity += newQuantity;
    }

    bool isShippable(){
        return shippingInfo != NULL;
    }

    bool isExpirable(){
        return expiryInfo != NULL;
    }

    ~Product(){
        delete shippingInfo;
        delete expiryInfo;
    }

};


class Customer{
    float balance;
public:
    Customer(float balance): balance(balance){};

    float getBalance(){
        return balance;
    }

    void UpdateBalance(float newBalance){
        balance += newBalance;        
    }

    bool canBuy(float price){
        return balance >= price;
    }

};

class  CartItem{
public:
    Product* product;
    int quantity;
    CartItem(Product* p, int q) : product(p), quantity(q){};
};

class Cart{
    vector<CartItem> cartItems;
public:

    void add(Product &item, int quantity){
        if( item.getQuantity() < quantity ){
            throw runtime_error("Not Sufficent Stocks");
        }

        if(item.isExpirable() && item.expiryInfo->isExpired()){
            throw runtime_error("Expired item, can\'t be added");
        }

        CartItem temp(&item, quantity);
        cartItems.push_back(temp);
    }

    int size(){
        return cartItems.size();
    }
    bool empty(){
        return cartItems.empty();
    }

    vector<CartItem>& getItems(){
        return this->cartItems;
    }


    float calSubtotal(){
        float subTotal = 0;
        for(int i=0; i<cartItems.size(); i++){
            float price =  cartItems[i].product->getPrice();
            int quantity = cartItems[i].quantity;
            subTotal += price * quantity;
        }
        return subTotal;
    }
};

class ShippingService{
    vector<CartItem> items;
public:
    ShippingService(vector<CartItem> ShippedItems): items(ShippedItems){}

    double getShippingFees(){
        /*Not sure if there's a logic behind shippign fees that i'm missing from the question*/
        return 30;
    }

    void shipItmes(){
        double totalWeight = 0;
        string unit;
        cout<< "**Shipment Notice **\n";
        for(int i=0; i<items.size(); i++){
            cout<< items[i].quantity << "x"<<" "<< items[i].product->getName()<<"\t";
            
            double weight = items[i].product->shippingInfo->getWeight();
            totalWeight += weight;
            unit = "g";
            if(weight >= 1000){ 
                weight /= 1000;
                unit = "kg";
            }
            cout<< weight<<unit<<endl;
        }

        if(totalWeight>1000){
            totalWeight /= 1000;
            unit = "kg";
        }

        cout<<"Total package weight "<< totalWeight<<unit<<endl;
    }

};


class CheckoutService{
public:
    void finalizeCheckout(vector<CartItem> &items ){
        for(int i=0; i < items.size(); i++){
            items[i].product->updateQuantity(-items[i].quantity) ;
        }
    }

    void checkout(Customer &customer, Cart &cart){
        if(cart.empty()){
            throw runtime_error("Empty Cart");
        }

        vector<CartItem> &items = cart.getItems();
        vector<CartItem> shippedItems;
       
        for(int i=0; i < items.size(); i++){
            if(items[i].product->isShippable()){
                shippedItems.push_back(items[i]);
            }
        }

        float subTotal = cart.calSubtotal();
        double shippingFees = 0;

        if(!shippedItems.empty()){
            ShippingService ship(shippedItems);
            ship.shipItmes();
            shippingFees = ship.getShippingFees();
        }

        double total = subTotal + shippingFees;

        if (customer.canBuy(total)){
            finalizeCheckout(items);
            printReceipt(items, subTotal, shippingFees, total);
            customer.UpdateBalance(-total);
        }else{
            throw runtime_error("Insufficent Funds");
        }
    }

    void printReceipt(vector<CartItem> items, double subtotal, double fees, double amount){
        cout<<" ** Checkout Recipt ** \n";
        for(int i=0; i<items.size(); i++){
            cout<< items[i].quantity<<"x "<< items[i].product->getName()<<"\t"<< items[i].product->getPrice()<<endl;
        }
        cout<<"-----------------------"<<endl
        <<"Subtotal\t"<<subtotal<<endl
        <<"Shipping fees\t"<<fees<<endl
        <<"Amount\t"<<amount<<endl;
    }
};

int main(){
    Customer seif(1500);

    Product* scratchCard= new Product("scratchCard", 10, 50);
    Product* cheese = new Product("cheese",5,10);
        cheese->shippingInfo = new ShippingInfo(400);
        cheese->expiryInfo = new ExpiryInfo("2025-08-18");
    Product* TV = new Product("tv", 500, 3);
        TV -> shippingInfo = new ShippingInfo(1100);
    Product* biscuits = new Product("biscuits",3, 30 );
        biscuits -> expiryInfo = new ExpiryInfo("2025-08-13");
    Cart cart;
    cart.add(*scratchCard, 5);
    cart.add(*cheese, 5);
    cart.add(*TV, 1);
    cart.add(*biscuits, 5);


    CheckoutService c;
    c.checkout(seif ,cart);

    cout<< "\nYour balance is now "<<seif.getBalance();

    system("pause");
    

}   