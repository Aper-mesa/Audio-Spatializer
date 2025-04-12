using UnityEngine;

[RequireComponent(typeof(AudioSource))]
public class SpatialAudioPlayer : MonoBehaviour
{
    public AudioClip[] tracks;

    private AudioSource source;

    void Start()
    {
        source = GetComponent<AudioSource>();
        source.spatialBlend = 1.0f;
        source.minDistance = 1f;
        source.maxDistance = 20f;
        source.rolloffMode = AudioRolloffMode.Logarithmic;

        if (tracks.Length > 0)
        {
            int index = Random.Range(0, tracks.Length);
            source.clip = tracks[index];
            source.Play();
        }
    }

    void Update()
    {
        //Rotate
        transform.position = new Vector3(Mathf.Sin(Time.time) * 2f, 1f, Mathf.Cos(Time.time) * 2f);
    }
}
