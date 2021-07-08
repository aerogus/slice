RM := rm -rf

CPP_SRCS += \
handleError.cpp \
sliceClass.cpp \
slice.cpp 

OBJS += \
handleError.o \
sliceClass.o \
slice.o 

CPP_DEPS += \
handleError.d \
sliceClass.d \
slice.d 

%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ --std=gnu++11 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

all: slice

slice: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ --std=gnu++11 -o "slice" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) slice
	-@echo ' '

.PHONY: all clean dependents
