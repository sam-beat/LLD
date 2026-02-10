import java.util.List;
import java.util.ArrayList;
import java.util.Collections;

final class Person{
    private final String name;
    private final int age;
    private final List<String>hobbies;

    public Person(String name, int age, List<String> hobbies){
        this.name = name;
        this.age = age;

        this.hobbies = new ArrayList<>(hobbies);
    }
    
    public void addHobbies(String hobby){
        hobbies.add(hobby);
    }
    public String getName(){
        return name;
    }

    public int getAge(){
        return age;
    }

    public List<String> getHobbies(){
        return Collections.unmodifiableList(hobbies);
    }

}

public class App {
    public static void main(String[] args) throws Exception {
        System.out.println("Hello, World!");

        List<String> h = new ArrayList<>();
        h.add("Cooking");
        h.add("Sleeping");
        
        Person person = new Person("parisaam", 26, h);

        System.out.println(person.getHobbies());
         h.add("Sleeping");
         person.addHobbies("BGMI");
         System.out.println(person.getHobbies());
         //person.getHobbies().add("abc");
         //person.setName("Sambit");
         
        
    }
}
