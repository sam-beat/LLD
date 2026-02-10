//Classic Example Payment Processing using different methods

interface PaymentStrategy{
    int pay(double amount);
}

class Cash implements PaymentStrategy{
    @Override
    public int pay(double amount){
        System.out.println("Paid " + amount + " using cash.." );
        return 1;
    }
}

class CreditCardPayment implements PaymentStrategy{
    private String creditCardNum;
    CreditCardPayment(String creditCardNum){
        this.creditCardNum = creditCardNum;
    }

    @Override
    public int pay(double amount){
        System.out.println(creditCardNum + " paid " + amount + " using credit card...");
        return 1;
    }
}

class UpiPayment implements PaymentStrategy{
    private String upiId;

    UpiPayment(String upiId){
        this.upiId = upiId;
    }

    @Override
    public int pay(double amount){
        System.out.println(upiId + " Paid " + amount + " using upi...");
        return 1;
    }
}

/*One payment object. No use of multiple if/else block. Based on the instance passed to the method the correct payment type is selected for payment */
public class App {
    public static void main(String[] args) throws Exception {
        System.out.println("Hello, World!");

        ProcessPayment payment = new ProcessPayment();
        payment.setPaymentStrategy(new Cash());
        payment.checkout(190);

        payment.setPaymentStrategy(new CreditCardPayment("1234 2345 4567 6789"));
        payment.checkout(12344);

        payment.setPaymentStrategy(new UpiPayment("1234567890@upi"));
        payment.checkout(20);

       
    }
}
