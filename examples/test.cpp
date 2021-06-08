#include "map"
#include "array"
#include "iostream"

#include "TObject.h"
#include "TFile.h"

struct StringArray : public std::array<std::string, 2>
{
  StringArray() = default;
  StringArray(std::string f, std::string s) {
    this->at(0) = std::move(f);
    this->at(0) = std::move(s);
  }
  ~StringArray() = default;
 private:
  ClassDef(StringArray, 1);
};
ClassImp(StringArray)

class TestClass : public TObject {
 public:
  TestClass(){
    StringArray test_array("aaaaaa", "bbbb");
//    std::array<std::string, 2> test_array{"aaaaaa", "bbbb"};
    test_map_[test_array] = "cccc";
  }

  void Print(Option_t *option="") const {
    for(const auto& element : test_map_){
      std::cout << element.first[0] << " " << element.first[1] << " " << element.second << std::endl;
    }
  }
 private:
  std::map<StringArray, std::string> test_map_{};
//  std::map<std::array<std::string, 2>, std::string> test_map_{};
  ClassDef(TestClass, 1);
};
ClassImp(TestClass)

void test(){
  auto* test_obj = new TestClass;
  test_obj->Print();

  auto* file = TFile::Open("test.root", "recreate");
  test_obj->Write("obj");
  file->Close();

  delete file;
  delete test_obj;

  file = TFile::Open("test.root", "read");
  test_obj = file->Get<TestClass>("obj");
  test_obj->Print();

  file->Close();
  delete file;
}

int main(int argc, char* argv[]) {
  test();
  return 0;
}
