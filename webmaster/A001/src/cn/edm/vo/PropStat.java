package cn.edm.vo;

public class PropStat {
	
	private PropertyVoStat industryStat;
	private PropertyVoStat societyStat;
	private PropertyVoStat ageStat;
	private PropertyVoStat genderStat;
	private int totalEmailNum = 0;
	public PropertyVoStat getIndustryStat() {
		return industryStat;
	}
	public void setIndustryStat(PropertyVoStat industryStat) {
		this.industryStat = industryStat;
	}
	public PropertyVoStat getSocietyStat() {
		return societyStat;
	}
	public void setSocietyStat(PropertyVoStat societyStat) {
		this.societyStat = societyStat;
	}
	public PropertyVoStat getAgeStat() {
		return ageStat;
	}
	public void setAgeStat(PropertyVoStat ageStat) {
		this.ageStat = ageStat;
	}
	public PropertyVoStat getGenderStat() {
		return genderStat;
	}
	public void setGenderStat(PropertyVoStat genderStat) {
		this.genderStat = genderStat;
	}
	public int getTotalEmailNum() {
		return totalEmailNum;
	}
	public void setTotalEmailNum(int totalEmailNum) {
		this.totalEmailNum = totalEmailNum;
	}
	
}
