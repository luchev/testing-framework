//
// Created by lyubo on 13.05.21 г..
//

#include <catch.hpp>

#include <Comparable.hpp>
#include <Debug.hpp>
#include <Serializable.hpp>
#include <Object.hpp>
#include <Document.hpp>
#include <KeyValueDatabase.hpp>

TEST_CASE("Comparable:0.2") {
    class ComparableImplementation: public Comparable {
    public:
        ComparableImplementation(const int foo = 0): foo(foo) {}
        bool operator==(const Comparable* other) const override {
            auto* other_casted = dynamic_cast<const ComparableImplementation*>(other);
            return this->foo == other_casted->foo;
        }
        bool operator!=(const Comparable* other) const override {
            return !(*this == other);
        }
    private:
        int foo;
    };

    Comparable* first = new ComparableImplementation(5);
    Comparable* second = new ComparableImplementation(5);
    Comparable* third = new ComparableImplementation(3);

    REQUIRE(first->operator==(second));
    REQUIRE(first->operator!=(third));

    delete first;
    delete second;
    delete third;
}

TEST_CASE("Debug:0.2") {
    class DebugImplementation: public Debug {
    public:
        DebugImplementation(const int foo = 0): foo(foo) {}
        std::string debug_print() const override {
            return std::to_string(this->foo);
        }
    private:
        int foo;
    };

    Debug* debug = new DebugImplementation(7);
    REQUIRE(debug->debug_print() == "7");
}

TEST_CASE("Serializable:0.2") {
    class SerializableImplementation: public Serializable {
    public:
        SerializableImplementation(const int foo, const double& bar): foo(foo), bar(bar) {}

        std::string to_string() const override {
            return std::to_string(this->foo) + "\n" + std::to_string(this->bar);
        }
        void from_string(const std::string& serialized_object) override {
            int pos = serialized_object.find('\n');

            std::string first_part = serialized_object.substr(0, pos);
            std::string second_part = serialized_object.substr(pos+1);

            this->foo = std::stoi(first_part);
            this->bar = std::stod(second_part);
        }
    private:
        int foo;
        double bar;
    };

    Serializable* serializable = new SerializableImplementation(5, 3.14);

    SECTION("to_string:0.1") {
        REQUIRE(serializable->to_string() == "5\n3.140000");
    }

    SECTION("from_string:0.1") {
        serializable->from_string("3\n5.14");
        REQUIRE(serializable->to_string() == "3\n5.140000");
    }
}

TEST_CASE("Object:1.0") {
    class File: public Object {
    public:
        File() = default;
        File(const std::string& name, const std::string& location, const std::string& extension): Object(name, location, extension){}

        bool operator==(const Comparable* other) const override {
            auto* other_casted = dynamic_cast<const Object*>(other);
            return this->get_fullpath() == other_casted->get_fullpath();
        }
        bool operator!=(const Comparable* other) const override {
            return !(*this == other);
        }

        std::string debug_print() const override {
            return this->get_fullpath();
        }

        std::string to_string() const override {
            return this->name;
        }
        void from_string(const std::string& serialized_object) override {
            this->name = serialized_object;
        }
        Object* clone() const {
            return new File(*this);
        }
    };

    Object* file = new File("file1", "/tmp", "txt");

    SECTION("get_name:0.1") {
        REQUIRE(file->get_name() == "file1");
    }

    SECTION("get_location:0.1") {
        REQUIRE(file->get_location() == "/tmp");
    }

    SECTION("get_extension:0.1") {
        REQUIRE(file->get_extension() == "txt");
    }

    SECTION("get_fullpath:0.1") {
        REQUIRE(file->get_fullpath() == "/tmp/file1.txt");
    }

    SECTION("Object as Comparable:0.2") {
        auto* first = static_cast<Comparable*>(file);
        Comparable* second = new File("file1", "/tmp", "txt");
        Comparable* third = new File("file2", "/tmp", "txt");

        REQUIRE(first->operator==(second));
        REQUIRE(first->operator!=(third));
    }

    SECTION("Object as Debug:0.2") {
        auto* object_as_debug = static_cast<Debug*>(file);
        REQUIRE(object_as_debug->debug_print() == "/tmp/file1.txt");
    }

    SECTION("Object as Serializable:0.2") {
        auto* object_as_serializable = static_cast<Serializable*>(file);
        REQUIRE(object_as_serializable->to_string() == "file1");

        object_as_serializable->from_string("file2");
        REQUIRE(object_as_serializable->to_string() == "file2");
    }
}

TEST_CASE("Document:1.0") {
    Document document("file1", "/tmp1", "doc");

    SECTION("Write & Read:0.15") {
        document.write_line("First line");
        document.write_line("Second line");
        document.write_line("Third line");

        std::string read = document.read_line();
        REQUIRE(read == "First line");

        read = document.read_line();
        REQUIRE(read == "Second line");

        read = document.read_line();
        REQUIRE(read == "Third line");

        REQUIRE_THROWS_AS(document.read_line(), std::out_of_range);

        document.write_line("Forth line");
        read = document.read_line();
        REQUIRE(read == "Forth line");
    }

    SECTION("Empty read:0.1") {
        document = Document("file1", "/tmp1", "doc");
        REQUIRE_THROWS_AS(document.read_line(), std::out_of_range);
    }

    SECTION("Write & read line:0.15") {
        document = Document("file1", "/tmp1", "doc");

        document.write_line("First line");
        document.write_line("Second line");
        document.write_line("Third line");

        std::string read = document.read_line(2);
        REQUIRE(read == "Second line");

        read = document.read_line();
        REQUIRE(read == "Third line");

        read = document.read_line(1);
        REQUIRE(read == "First line");

        read = document.read_line();
        REQUIRE(read == "Second line");

        const unsigned line = 0;
        REQUIRE_THROWS_AS(document.read_line(line), std::out_of_range);

        const unsigned line2 = 10;
        REQUIRE_THROWS_AS(document.read_line(line2), std::out_of_range);

        const unsigned line3 = -1;
        REQUIRE_THROWS_AS(document.read_line(line3), std::out_of_range);
    }

    SECTION("Document as Object:0.1") {
        auto* document_as_object = dynamic_cast<Object*>(&document);
        REQUIRE_FALSE(document_as_object == nullptr);
    }

    Document first = Document("doc1", "/tmp", "doc");
    first.write_line("foo");
    first.write_line("bar");
    first.write_line("baz");

    Document second = Document("doc2", "/tmp", "doc");
    second.write_line("qwerty");
    second.write_line("asdf");
    second.write_line("zxcv");

    Document third = first;

    Document forth = first;
    forth.write_line("foobar");

    SECTION("operator ==, !=:0.1") {
        Comparable* first_as_comparable = &first;
        Comparable* second_as_comparable = &second;
        Comparable* third_as_comparable = &third;
        Comparable* forth_as_comparable = &forth;

        REQUIRE_FALSE(first_as_comparable->operator==(second_as_comparable));
        REQUIRE(first_as_comparable->operator!=(second_as_comparable));

        REQUIRE(first_as_comparable->operator==(third_as_comparable));
        REQUIRE(first_as_comparable->operator!=(forth_as_comparable));
    }

    SECTION("debug_print():0.1") {
        Debug* first_as_debug = &first;
        std::string result = first_as_debug->debug_print();
        REQUIRE(result == "Line 1:foo\nLine 2:bar\nLine 3:baz\n");
    }

    SECTION("to_string():0.15") {
        Serializable* first_as_serializable = &first;
        std::string result = first_as_serializable->to_string();

        std::string expected = "doc1\n/tmp\ndoc\nfoo\nbar\nbaz\n";
        REQUIRE(result == expected);
    }

    SECTION("from_string():0.15") {
        Serializable* new_document = new Document("asd", "asd", "asd");

        new_document->from_string("doc5\n/tmp/dir1\ndocx\nfoobar\nbaz\nfoobarbaz\n");

        auto* new_doc = dynamic_cast<Document*>(new_document);
        REQUIRE_FALSE(new_doc == nullptr);

        REQUIRE(new_doc->get_fullpath() == "/tmp/dir1/doc5.docx");

        REQUIRE(new_doc->read_line(1) == "foobar");
        REQUIRE(new_doc->read_line(2) == "baz");
        REQUIRE(new_doc->read_line(3) == "foobarbaz");
    }
}

TEST_CASE("KeyValueDatabase:1.0") {
    KeyValueDatabase db("db", "/tmp", "kvdb");

    SECTION("Add & get entry:0.2") {
        db.add_entry({"key1", 1});
        db.add_entry({"key2", 2});
        db.add_entry({"key3", 3});
        db.add_entry({"key44", 1233});

        int value = db.get_value("key1");
        REQUIRE(value == 1);

        value = db.get_value("key2");
        REQUIRE(value == 2);

        value = db.get_value("key1");
        REQUIRE(value == 1);

        value = db.get_value("key3");
        REQUIRE(value == 3);

        value = db.get_value("key44");
        REQUIRE(value == 1233);

        REQUIRE_THROWS_AS(db.get_value("asd"), std::invalid_argument);
    }

    SECTION("Add duplicate entry:0.2") {
        db = KeyValueDatabase("db", "/tmp", ".kvdb");

        db.add_entry({"key1", 1});
        db.add_entry({"key2", 1});

        REQUIRE_THROWS_AS(db.add_entry({"key1", 2}), std::invalid_argument);
    }

    SECTION("KeyValueDatabase as Object:0.1") {
        auto* document_as_object = dynamic_cast<Object*>(&db);
        REQUIRE_FALSE(document_as_object == nullptr);
    }

    KeyValueDatabase first("db1", "/tmp1", "kvdb");
    first.add_entry({"key1", 51});
    first.add_entry({"key2", 515});
    first.add_entry({"key3", 551});
    first.add_entry({"key4", 251});

    KeyValueDatabase second("db2", "/tmp1", "kvdb");
    second.add_entry({"key1", 51});
    second.add_entry({"key2", 335});
    second.add_entry({"key3", 287});
    second.add_entry({"key4", 741});


    KeyValueDatabase third = first;

    KeyValueDatabase forth = first;
    forth.add_entry({"key5", -2});

    SECTION("operator ==, !=:0.1") {
        Comparable* first_as_comparable = &first;
        Comparable* second_as_comparable = &second;
        Comparable* third_as_comparable = &third;
        Comparable* forth_as_comparable = &forth;

        REQUIRE_FALSE(first_as_comparable->operator==(second_as_comparable));
        REQUIRE(first_as_comparable->operator!=(second_as_comparable));

        REQUIRE(first_as_comparable->operator==(third_as_comparable));
        REQUIRE(first_as_comparable->operator!=(forth_as_comparable));
    }

    SECTION("debug_print():0.1") {
        Debug* first_as_debug = &first;
        std::string result = first_as_debug->debug_print();
        REQUIRE(result == "{key1:51}\n{key2:515}\n{key3:551}\n{key4:251}\n");
    }

    SECTION("to_string():0.15") {
        Serializable* first_as_serializable = &first;
        std::string result = first_as_serializable->to_string();

        std::string expected = "db1\n/tmp1\nkvdb\nkey1:51\nkey2:515\nkey3:551\nkey4:251\n";
        REQUIRE(result == expected);
    }

    SECTION("from_string():0.15") {
        Serializable* kvd_as_serializable = new KeyValueDatabase("asd", "asd", "asd");

        kvd_as_serializable->from_string("db1\n/tmp1/dir2\nkvdb\nkey1:51\nkey2:515\nkey3:551\nkey4:251\n");

        auto* kvd = dynamic_cast<KeyValueDatabase*>(kvd_as_serializable);
        REQUIRE_FALSE(kvd == nullptr);

        REQUIRE(kvd->get_fullpath() == "/tmp1/dir2/db1.kvdb");

        REQUIRE(kvd->get_value("key1") == 51);
        REQUIRE(kvd->get_value("key2") == 515);
        REQUIRE(kvd->get_value("key3") == 551);
        REQUIRE(kvd->get_value("key4") == 251);
    }
}