

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * 
 */
class TEST_API FTestCommands : public TCommands<FTestCommands>
{
public:
	FTestCommands();
	
	virtual void RegisterCommands() override;
	
	TSharedPtr< FUICommandInfo > CommandA;
};
