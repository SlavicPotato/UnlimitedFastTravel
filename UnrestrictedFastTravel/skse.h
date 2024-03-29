#pragma once

#include <ext/ISKSE.h>
#include <ext/ISKSESerialization.h>

class ISKSE :
	public ISKSEBase<
		SKSEInterfaceFlags::kMessaging |
			SKSEInterfaceFlags::kTrampoline |
			SKSEInterfaceFlags::kPapyrus,
		256,
		512>
{
public:
	[[nodiscard]] inline static constexpr auto& GetSingleton()
	{
		return m_Instance;
	}

	[[nodiscard]] inline static constexpr auto& GetBranchTrampoline()
	{
		return m_Instance.GetTrampoline(TrampolineID::kBranch);
	}

	[[nodiscard]] inline static constexpr auto& GetLocalTrampoline()
	{
		return m_Instance.GetTrampoline(TrampolineID::kLocal);
	}

private:
	ISKSE() = default;

	virtual void OnLogOpen() override;
	virtual const char* GetLogPath() const override;
	virtual const char* GetPluginName() const override;
	virtual std::uint32_t GetPluginVersion() const override;
	virtual bool CheckRuntimeVersion(std::uint32_t a_version) const override;

	static ISKSE m_Instance;
};