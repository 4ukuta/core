package ru.spb.osll.airomo;

import java.util.List;

import org.json.JSONObject;

import ru.spb.osll.json.IRequest.IResponse;
import ru.spb.osll.json.*;
import ru.spb.osll.objects.Mark;
import ru.spb.osll.preferences.Settings.ITrackerNetSettings;
import ru.spb.osll.utils.TrackerUtil;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class Ala extends BaseAla {
	private boolean m_isWorking = false;
	private int m_trackInterval;
	private Buffer<Mark> m_history;
	
	public Ala(Context c) {
		super(c);
		m_trackInterval = getTrackInterval();
		m_history = new Buffer<Mark>(getHistoryLimit());
	}
	
	@Override
	public void onDestroy(Context c) {
		super.onDestroy(c);
		if (m_trackThread != null){
			m_trackThread.interrupt();
		}
		if (m_historyThread != null){
			m_historyThread.interrupt();
		}
		stopLocationListener();
	}

	private synchronized Buffer<Mark> getHistory(){
		return m_history;
	}

	@Override
	public String auth(String login, String pass) {
		String authToken = null;
		final String serverUrl = netData().serverUrl;
		JSONObject JSONResponse = new JsonLoginRequest(login, pass, serverUrl).doRequest();
		if (JSONResponse != null) {
			authToken = JsonBase.getString(JSONResponse, IResponse.AUTH_TOKEN);
		} else {
			onErrorOccured(TrackerUtil.MESS_FAIL_CONNECTION_TO_SERVER);
		}
		return authToken;
	}

	@Override
	public void startTrack(Context c) {
		m_isWorking = true;
//		startLocationListener();
//		doTracking();

		c.startService(new Intent(c, AlaService.class));
	}

	@Override
	public void stopTrack(Context c) {
		m_isWorking = false;
//		dropCache();
//		stopLocationListener();

		c.stopService(new Intent(c, AlaService.class));
	}

	@Override
	public boolean isTracking(Context c) {
		return m_isWorking;
	}
	
	@Override
	public void setUser(String login) {
		setPreference(ITrackerNetSettings.LOGIN, login);
	}

	@Override
	public String getUser() {
		return getPreference(ITrackerNetSettings.LOGIN, "?????");
	}

	@Override
	public void setPass(String pass) {
		setPreference(ITrackerNetSettings.PASSWORD, pass);
	}

	@Override
	public String getPass() {
		return getPreference(ITrackerNetSettings.PASSWORD, "?????");
	}	

	@Override
	public void setTrackInterval(int sec) {
		m_trackInterval = sec;
		setPreference(ITrackerNetSettings.TIME_TICK, sec);
	}

	@Override
	public int getTrackInterval() {
		return getPreference(ITrackerNetSettings.TIME_TICK, 5);
	}

	@Override
	public void setHistoryLimit(int maxMarks) {
		getHistory().setBufferSize(maxMarks);
		setPreference(ITrackerNetSettings.HISTORY_LIMIT, maxMarks);
	}

	@Override
	public int getHistoryLimit() {
		return getPreference(ITrackerNetSettings.HISTORY_LIMIT, 50);
	}
	
	@Override
	public void setServerUrl(String severUrl) {
		netData().serverUrl = severUrl;
		setPreference(ITrackerNetSettings.SERVER_URL, severUrl);
	}

	@Override
	public String getServerUrl() {
		return getPreference(ITrackerNetSettings.SERVER_URL, "?????");
	}

	@Override
	public void setChannel(String channel) {
		setPreference(ITrackerNetSettings.CHANNEL, channel);
	}

	@Override
	public String getChannel() {
		return getPreference(ITrackerNetSettings.CHANNEL, "?????");
	}
	
	@Override
	protected void networkStatusChanged(boolean isOnline) {
		if(isOnline){
			sendHistory();
		} else {
			dropCache();			
		}
	}

	@Override
	protected void gooffEvent() {
		sendLastCoordinate();
		sendHistory();
	}
	
	private String m_authTokenCache = null;
	private boolean m_isChanAvailableCache = false;
	private void dropCache(){
		m_authTokenCache = null;
		m_isChanAvailableCache = false;
	}
	
	private Thread m_historyThread;
	@Override
	public void sendHistory() {
		if  (m_historyThread != null){
			m_historyThread.interrupt();
		}
		m_historyThread = new Thread(new Runnable() {
			@Override
			public void run() {
				refreshConnectionData();
				try {
					while(isOnline() && getHistory().hasElements()){
						Log.v(ALA_LOG, "sending history...");
						final Mark mark = getHistory().getFirst();
						if (sendMark(mark)){
							Log.v(ALA_LOG, "sending history: mark was sent...");
							getHistory().removeFirst();
						}
						Thread.sleep(333);
					}
				} catch (InterruptedException e) {
					Log.v(ALA_LOG, "m_historyThread is interrupted");
				}
			}
		});
		m_historyThread.start();
	}

	@Override
	public void sendLastCoordinate() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				refreshConnectionData();
				final Mark mark = constructDruftMark();
				sendMark(mark);
			}
		}).start();
	}

	@Override
	public List<Mark> getAllMarks(){
		return getHistory().getBufferData();
	}

	private Thread m_trackThread;
	private void doTracking(){
		if (m_trackThread != null){
			m_trackThread.interrupt();
		}
		m_trackThread = new Thread(new Runnable() {
			@Override
			public void run() {
				refreshConnectionData();
				try {
					while (!Thread.currentThread().isInterrupted() && m_isWorking){
						final Mark mark = constructDruftMark();
						if (isOnline()){
							Log.v(ALA_LOG, "send mark to server...");
							sendMark(mark);
						} else {
							Log.v(ALA_LOG, "add mark to history...");
							getHistory().add(mark);
						}
						Thread.sleep(m_trackInterval * 1000);
					}
				} catch (InterruptedException e) {
					Log.v(ALA_LOG, "m_trackThread is interrupted");
				}
			}
		});
		m_trackThread.start();
	}
	
	private boolean sendMark(Mark mark){
		if(!m_isChanAvailableCache){
			m_authTokenCache = auth(netData().login, netData().pass);
			m_isChanAvailableCache = applyChannel(m_authTokenCache, netData().channel);
		}
		if (m_isChanAvailableCache){
			completeMark(mark, m_authTokenCache, netData().channel);
			return applyMark(mark);
		}
		return false;
	}
	
	private boolean applyChannel(String authToken, String channel){
		Log.v(ALA_LOG, "applayChannel");
		JSONObject JSONResponse = new JsonApplyChannelRequest(authToken, channel,
				"Geo2Tag tracker channel", "http://osll.spb.ru/", 3000, netData().serverUrl)
				.doRequest();
		boolean success = false;
		if(JSONResponse != null){
			String status = JsonBase.getString(JSONResponse, IResponse.STATUS);
			String statusDescription = JsonBase.getString(JSONResponse, IResponse.STATUS_DESCRIPTION);
			success = status.equals(IResponse.OK_STATUS) || statusDescription.equals(IResponse.CHANNEL_EXTSTS); 
			if (!success){
				onErrorOccured(TrackerUtil.MESS_FAIL_APPLY_CHANNEL + statusDescription);
			}
		} else {
			onErrorOccured(TrackerUtil.MESS_FAIL_CONNECTION_TO_SERVER);
		}
		return success;
	}
	
	private boolean applyMark(Mark mark){
		boolean success = false;
		JSONObject JSONResponse = new JsonApplyMarkRequest(mark, netData().serverUrl).doRequest();
		if (JSONResponse != null){
			String status = JsonBase.getString(JSONResponse, IResponse.STATUS);
			String statusDescription = JsonBase.getString(JSONResponse, IResponse.STATUS_DESCRIPTION);
			success = status.equals(IResponse.OK_STATUS);
			if (success){
				Log.v(ALA_LOG, "sent " + TrackerUtil.convertLocation(mark));
			} else {
				onErrorOccured("apply mark:" + status + "," + statusDescription);
			}
		} 
		return success;
	}
}
