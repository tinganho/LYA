
#include <vector>
#include <codecvt>
#include <tuple>
#include "extension_server/extension_server.h"
#include "javascript/extractor.h"
#include "javascript/compiler.h"

using namespace Lya::services;
using namespace Lya::lib::types;
using namespace Lya::javascript_extension;

class VehicleVisitor;
class Vehicle {
public:
	Vehicle(int wheels, std::string name):
			wheels(wheels),
			name(name) { }

	std::string name;
	int wheels;
	virtual void accept(VehicleVisitor*);
};

class Car : public Vehicle {
public:
	Car(std::string name): Vehicle(4, name), wonk("Tut tut") { }

	std::string wonk;
	void accept(VehicleVisitor*);
};


class VehicleVisitor
{
public:
	virtual void visit(const Vehicle*) = 0;
	virtual void visit(const Car*) = 0;
};

class PrintVisitor final : public VehicleVisitor {
	void visit(const Vehicle* vehicle)
	{
		std::cout << vehicle->name << std::endl;
	}

	void visit(const Car* car)
	{
		std::cout << car->wonk << std::endl;
	}
};

void Vehicle::accept(VehicleVisitor* visitor)
{
	visitor->visit(this);
};

void Car::accept(VehicleVisitor* visitor)
{
	visitor->visit(this);
}

std::unique_ptr<Vehicle> create_car()
{
	return std::make_unique<Car>("MyCar");
}

std::tuple<std::vector<LocalizationLocation>, std::vector<Diagnostic>> extract(const string &file, const vector<string> &function_names, uint64_t start_line) {
    JavaScriptLocalizationExtractor extractor(file, function_names, JavaScriptLanguage::JavaScript);
    return extractor.extract(start_line);
}

std::vector<Diagnostic> compile(const std::vector<std::string>& localization_files) {
	Compiler compiler(JavaScriptLanguage::JavaScript, 0, 0);
	return compiler.compile(localization_files, "en-US");
}

int main() {
	std::unique_ptr<Vehicle> vehicle = create_car();
	PrintVisitor* print_visitor = new PrintVisitor();
	vehicle->accept(print_visitor);
    ExtensionServer extension_server("localhost:8888", extract, compile);
    extension_server.start_server(false);
    return 0;
}
